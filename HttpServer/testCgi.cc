#include <iostream>
using namespace std;
#include <unistd.h>
#include <stdlib.h>
#include "Util.hpp"
int GetData(string& str)
{
	size_t pos=str.find('=');
	if(string::npos==pos)
	{
		return Util::StringToInt(str.substr(pos+1));
	}
}
int main()
{
	string args;
	string method=getenv("METHOD");
	if(method=="GET")
		args=getenv("QUERY_STRING");
	else if(method=="POST")
	{
		string cl=getenv("CONTENT_LENGTH");
		int content_length=Util::StringToInt(cl);
		char c;
		while(content_length--)
		{
			read(0,&c,1);
			args.push_back(c);
		}
	}
	else
	{}
	size_t pos=args.find('&');
	if(string::npos==pos)
	{
		string first=args.substr(0,pos);
		string second=args.substr(pos+1);
		int data1=GetData(first);
		int data2=GetData(second);
		cout<<data1<<"+"<<data2<<"="<<data1+data2<<endl;
		cout<<data1<<"-"<<data2<<"="<<data1-data2<<endl;
		cout<<data1<<"*"<<data2<<"="<<data1*data2<<endl;
		cout<<data1<<"/"<<data2<<"="<<data1/data2<<endl;
	}
	return 0;
}
