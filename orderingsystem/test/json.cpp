//演示jsoncpp的基本使用：序列化和反序列化的过程
#include <cstdio>
#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

int main()
{
	std::string name="小明";
	std::string gender="男";
	int age=20;

	Json::Value value;
	value["姓名"]=name;
	value["性别"]=gender;
	value["年龄"]=age;
	value["成绩"].append(80);
	value["成绩"].append(77);
	value["成绩"].append(90);

	//序列化
	Json::StyledWriter writer;
	std::string json_str=writer.write(value);
	printf("json_str:[%s]\n",json_str.c_str());

	Json::FastWriter fwriter;
	json_str=fwriter.write(value);
	printf("json_str:[%s]\n",json_str.c_str());

	//反序列化
	Json::Value val2;
	Json::Reader reader;//json反序列化对象，将json数据串解析成为各个数据对象
	reader.parse(json_str,val2);//将数据串进行解析，放到val2中
	std::cout<<"name:"<<val2["姓名"]<<std::endl;
	std::cout<<"gender:"<<val2["性别"]<<std::endl;
	std::cout<<"age:"<<val2["年龄"].asInt()<<std::endl;
	std::cout<<"score:"<<val2["成绩"]<<std::endl;

	for(auto it : val2["成绩"])//遍历获取各科成绩
	{
		std::cout<<"ch score:"<<it<<std::endl;
	}

}
