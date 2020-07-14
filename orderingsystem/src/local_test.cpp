#include "order_system.hpp"
int main()
{
	MYSQL *mysql=_order_sys::MysqlInit();
	if(mysql==NULL)
		return -1;
	
	_order_sys::DishTable dish_tb(mysql);
	_order_sys::OrderTable order_tb(mysql);

/*
	//插入菜品信息测试
	//插入两条信息
	Json::Value dish;
	dish["name"]="炝莲菜";
	dish["price"]=25;
	dish_tb.Insert(dish);
	Json::Value dish0;
	dish0["name"]="土豆炖鸡块";
	dish0["price"]=35;
	dish_tb.Insert(dish0);
*/
/*
	//查询菜品信息测试
	Json::Value dish1;
	Json::StyledWriter writer;
	dish_tb.SelectOne(14,&dish1);
	std::cout<<"dish:"<<writer.write(dish1);
*/
/*
	Json::Value dish2;
	Json::StyledWriter writer2;
	dish_tb.SelectAll(&dish2);
	std::cout<<"dish:"<<writer2.write(dish2);
*/	
/*
	//更新菜品信息测试
	Json::Value dish3;
	dish3["id"]=14;
	dish3["name"]="盐煎肉";
	dish3["price"]=68;
	dish_tb.Update(dish3);
*/
/*
	//删除菜品信息
	dish_tb.Delete(14);
*/
/*
	//插入订单信息测试
	Json::Value order;
	Json::StyledWriter writer;
	order["table_id"]=5;
	order["dishes"].append(14);
	order["dishes"].append(15);
	order["status"]=0;
	order_tb.Insert(order);
*/
/*
	//查询订单信息测试
	Json::Value order1;
	Json::StyledWriter writer;
	order_tb.SelectOne(1,&order1);
	std::cout<<"order:"<<writer.write(order1);
*/
/*	
	//查询所有订单信息测试
	Json::Value order;
	Json::StyledWriter writer;
	order_tb.SelectAll(&order);
	std::cout<<"order:"<<writer.write(order);
*/
/*
	//更新订单信息测试
	Json::Value order;
	order["id"]=1;
	order["table_id"]=3;
	order["dishes"].append(14);
	order["status"]=0;
	order_tb.Update(order);
*/
	_order_sys::MysqlDestroy(mysql);
	return 0;
}

