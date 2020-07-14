#include "httplib.h"
#include <iostream>

void HelloWorld(const httplib::Request &req,httplib::Response &rsp)
{
	std::cout<<"method:"<<req.method<<std::endl;
	std::cout<<"path:"<<req.path<<std::endl;
	std::cout<<"body:"<<req.body<<std::endl;
	rsp.status=200;//响应状态码
	rsp.body="<html><body><h1>HelloWorld</h1></body></html>";
	rsp.set_header("Content-Type","text/html");

	//std::string body="<html><body><h1>HelloWorld<</h1></body></html>";
	//rsp.set_content(body,body.size(),"text/html");
	return ;
}
int main()
{
	int a = 100;
	httplib::Server server;//实例化Server对象
	server.Get("/helloworld",HelloWorld);
	server.Get("/hi",[&](const httplib::Request &req,httplib::Response &rsp){
			std::cout<<"a:"<<a<<std::endl;
			std::string body="<html><body><h1>Hi</h1></body></html>";
			rsp.set_content(body.c_str(),body.size(),"text/html");
			});

	server.listen("0.0.0.0",9000);//搭建http服务器
	return 0;
}
