#include "order_system.hpp"
#include "httplib.h"

_order_sys::DishTable *dish_table;
_order_sys::OrderTable *order_table;//定义两个对象

//菜单回调函数
void InsertDish(const httplib::Request &req,httplib::Response &rsp)
{
	//req.body中保存正文，正文就是菜品信息的json数据串
	Json::Value dish_val;
	Json::Reader reader;//解析
	bool ret=reader.parse(req.body,dish_val);
	if(ret==false)
	{
		std::cout<<"InsertDish parse json data failed!!\n";
		rsp.status=400;//状态码
	}
	//解析成功，插入
	ret=dish_table->Insert(dish_val);
	if(ret == false)
	{
		rsp.status=500;
		return ;
	}
	rsp.status=200;
	return ;
}
void DeleteDish(const httplib::Request &req,httplib::Response &rsp)
{
	//  /dish(\d+)
	//matches中保存所有匹配的指定字符串
	int dish_id = std::stoi(req.matches[1]);//匹配 /dish/数字 这种格式的字符串
	bool ret=dish_table->Delete(dish_id);
	if(ret==false)
	{
		rsp.status=500;
		return ;
	}
	rsp.status=200;
	return ;
}
void UpdateDish(const httplib::Request &req,httplib::Response &rsp)
{
	//新的菜品信息保存在请求正文中，更新到数据库中
	Json::Value dish_val;
	Json::Reader reader;
	bool ret=reader.parse(req.body,dish_val);
	if(ret==false)
	{
		rsp.status=400;
		return ;
	}
	//更新数据库前，要对信息进行服务器端校验
	ret=dish_table->Update(dish_val);
	if(ret==false)
	{
		rsp.status=500;
		return ;
	}
	rsp.status=200;
	return ;
}
void SelectOneDish(const httplib::Request &req,httplib::Response &rsp)
{
	int dish_id=std::stoi(req.matches[1]);
	//从数据库中获取指定id的菜品信息
	Json::Value dish_val;
	bool ret=dish_table->SelectOne(dish_id,&dish_val);
	if(ret==false)
	{
		rsp.status=500;
		return ;
	}
	//获取成功，将dish_val中的数据对象序列化成为json数据串，作为rsp.body响应
	Json::FastWriter writer;
	std::string body=writer.write(dish_val);
	rsp.set_content(body.c_str(),body.size(),"application/json");//设置响应正文信息
	rsp.status=200;
	return ;
}
void SelectAllDish(const httplib::Request &req,httplib::Response &rsp)
{
	Json::Value dishes_val;
	bool ret=dish_table->SelectAll(&dishes_val);
	if(ret==false)
	{
		rsp.status=500;
		return ;
	}
	Json::FastWriter writer;
	std::string body=writer.write(dishes_val);
	rsp.set_content(body.c_str(),body.size(),"application/json");
	rsp.status=200;
	return ;
}

//订单回调函数
void InsertOrder(const httplib::Request &req,httplib::Response &rsp)
{
	Json::Value order_val;
	Json::Reader reader;
	bool ret=reader.parse(req.body,order_val);
	if(ret==false)
	{
		std::cout<<"InsertOrder parse json failed!!\n";
		rsp.status=400;
		return ;
	}
	ret=order_table->Insert(order_val);
	if(ret==false)
	{
		rsp.status=500;
		return ;
	}
	rsp.status=200;
	return ;
}
void DeleteOrder(const httplib::Request &req,httplib::Response &rsp)
{
	int order_id=std::stoi(req.matches[1]);
	bool ret=order_table->Delete(order_id);
	if(ret==false)
	{
		std::cout<<"DeleteOrder delete order failed!!\n";
		rsp.status=500;//服务器内部错误
		return ;
	}
	rsp.status=200;
	return ;
}
void UpdateOrder(const httplib::Request &req,httplib::Response &rsp)
{
	Json::Value order_val;
	Json::Reader reader;
	bool ret=reader.parse(req.body,order_val);
	if(ret==false)
	{
		std::cout<<"UpdateOrder parse json failed!!\n";
		rsp.status=400;
		return ;
	}
	ret = order_table->Update(order_val);
	if(ret==false)
	{
		std::cout<<"UpdateOrder update data to db failed!!\n";
		rsp.status=500;
		return ;
	}
	rsp.status=200;
	return ;
}
void SelectOneOrder(const httplib::Request &req,httplib::Response &rsp)
{
	int order_id=std::stoi(req.matches[1]);
	Json::Value order_val;
	bool ret=order_table->SelectOne(order_id,&order_val);
	if(ret==false)
	{
		std::cout<<"SelectOneOrder get order info from db failed!!\n";
		rsp.status=500;
		return ;
	}
	Json::FastWriter writer;
	std::string body=writer.write(order_val);
	rsp.set_content(body.c_str(),body.size(),"application/json");
	rsp.status=200;
	return ;
}
void SelectAllOrder(const httplib::Request &req,httplib::Response &rsp)
{
	Json::Value orders_val;
	bool ret=order_table->SelectAll(&orders_val);
	if(ret==false)
	{
		std::cout<<"SelectAll get order info from db failed!!\n";
		rsp.status=500;
		return ;
	}
	Json::FastWriter writer;
	std::string body=writer.write(orders_val);
	rsp.set_content(body.c_str(),body.size(),"application/json");
	rsp.status=200;
	return ;
}

#define WWWROOT "./wwwroot"
int main()
{
	MYSQL *mysql=_order_sys::MysqlInit();
	dish_table = new _order_sys::DishTable(mysql);
	order_table = new _order_sys::OrderTable(mysql);//初始化两个对象:菜单对象和订单对象

	httplib::Server server;

	//添加路由表信息
	//插入菜品请求 POST
	server.Post("/dish",InsertDish);
	//删除菜品请求 DELETE
	server.Delete(R"(/dish/(\d+))",DeleteDish);
	//更新菜品请求 PUT
	server.Put("/dish",UpdateDish);
	//查询单个菜品请求 GET
	server.Get(R"(/dish/(\d+))",SelectOneDish);//使用正则表达式表示，R表示取出字符串中特殊字符的特殊含义，这里主要是为了取消中间的\的转义含义，\d+表示有一个或者多个数字字符，(\d+)表示捕捉这个数字
	//查询所有菜品请求 GET
	server.Get("/dish",SelectAllDish);
	
	//订单信息增删改查
	server.Post("/order",InsertOrder);
	server.Delete(R"(/order/(\d+))",DeleteOrder);
	server.Put("/order",UpdateOrder);
	server.Get(R"(/order/(\d+))",SelectOneOrder);
	server.Get("/order",SelectAllOrder);

	//server.set_base_dir(静态资源默认根路径)
	//设置静态资源根目录之后，如果浏览器请求的是根目录下的静态资源，httplib就会自动的将资源数据读取出来返回给浏览器
	server.set_base_dir(WWWROOT);//设置前端页面相对的根目录

	server.listen("0.0.0.0",9000);
	_order_sys::MysqlDestroy(mysql);
	return 0;
}
