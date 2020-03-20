#pragma once
#include <iostream>
using namespace std;
#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <utility>
#include <unordered_map>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "Util.hpp"
#define WWW "./wwwroot2"
#include <sys/wait.h>
//先构造http请求报文
class HttpRequest
{
	private:
		string request_line;//请求行
		string request_header;//请求报头
		string request_blank;//请求空行
		string request_body;//请求正文
	private:
		string method;//方法
		string url;//路径+参数
		string version;
		unordered_map<string,string> header_kv;//保存请求报头转换成的键值对

		string path;//资源路径
		string query_string;//GET方法上传的参数

		int resource_size;//资源长度
		string suffix;//后缀

		bool cgi;
		bool done;
	public:
		HttpRequest()
			:request_blank("\n")
			 ,path(WWW)
			 ,resource_size(0)
			 ,cgi(false)
			 ,done(false)
			 ,suffix(".html")
	{}
		bool GetDone()
		{
			return done;
		}
		void SetDone(bool _done)
		{
			done=_done;
		}
		string& GetRequestLine()//获取请求行
		{
			return request_line;
		}
		string& GetRequestHeader()//获取请求报头
		{
			return request_header;
		}
		string& GetRequestBody()//获取请求正文
		{
			return request_body;
		}
		string& GetQueryString()
		{
			return query_string;
		}
		string GetPath()//获取资源路径
		{
			return path;
		}
		string& GetMethod()
		{
			return method;
		}
		bool MethodIsLegal()//判断方法是否合法
		{
			if(method!="GET" && method!="POST")
				return false;
			return true;
		}
		int GetResourceSize()//获取资源长度
		{
			return resource_size;
		}
		string GetSuffix()//获取后缀
		{
			return suffix;
		}

		//开始解析请求行
		void RequestLineParse()//解析成method/url/version
		{
			//我们这里使用stringstream字符流来进行解析
			stringstream ss(request_line);//分隔
			ss>>method>>url>>version;
			//将方法转换为大写形式（因为GET和POST都是大写）
			Util::StringToUpper(method);
			cout<<"method: "<<method<<endl;
			cout<<"url: "<<url<<endl;
			cout<<"version: "<<version<<endl;
		}

		//开始对报文头进行解析
		void RequestHeaderParse()
		{
			//包含请求报头名称和请求报头值，因此需要使用键值对
			vector<string> v;
			Util::TransformToVector(request_header,v);//将报头转为vector形式存在v中
			auto it=v.begin();
			for(;it!=v.end();++it)
			{
				string k;
				string v;
				Util::MakeKV(*it,k,v);//将vector中国的string分解为k和v
				header_kv.insert(make_pair(k,v));//将键值对存入header_kv中以备使用
				cout<<"key: "<<k<<endl;
				cout<<"value: "<<v<<endl;
			}
		}

		//POST一定带参数，GET如果包含?，则一定带参数，不包含则不带参数
		//先对请求行的url进行解析
		void UrlParse()
		{
			if(method=="POST")
			{
				cgi=true;//带参
				path+=url;//资源路径加上url
			}
			else
			{
				size_t pos=url.find('?');
				if(string::npos==pos)//没有?，不带参
				{
					path+=url;
				}
				else//带参数
				{
					cgi=true;
					path+=url.substr(0,pos);
					query_string=url.substr(pos+1);
				}
			}
			if(path[path.size()-1]=='/')//要链接资源
			{
				path+="index.html";
			}
			//区分后缀
			size_t pos=path.rfind(".");//.后面的就是后缀
			if(string::npos==pos)
			{
				suffix="html";
			}
			else
			{
				suffix=path.substr(pos);
			}
			cout<<"debug:suffix: "<<suffix<<endl;
		}

		bool IsPathLegal()//检测路径是否合法
		{
			struct stat st;
			if(stat(path.c_str(),&st)==0)//说明路径合法
			{
				if(S_ISDIR(st.st_mode))//是目录
				{
					path+="/index.html";
				}
				else//不是目录
				{
					if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))//自己具有可执行，其他人具有可执行，所属组具有可执行权限
					{
						cgi=true;
					}
				}
				resource_size=st.st_size;
				return true;
			}
			else//路径不合法
			{
				return false;
			}
		}

		bool IsNeedRecv()//是否需要继续读
		{
			return method=="POST";//如果方法是POST要继续读
		}
		int GetContentLength()
		{
			auto it=header_kv.find("Content-Length");
			if(it==header_kv.end())
			{
				return -1;
			}
			return Util::StringToInt(it->second);
		}
		bool IsCgi()
		{
			return cgi;
		}
		void Make_404()
		{
			suffix=".html";
			path="wwwroot/404.html";
			struct stat st;
			stat(path.c_str(),&st);
			resource_size=st.st_size;
		}
		void ReMakeRequest(int code)//重新作出响应
		{
			switch(code)
			{
				case 400:
				case 404:
					Make_404();
					break;
				default:
					break;
			}
		}
		~HttpRequest()
		{}
};
class HttpResponse
{
	private:
		string response_line;//响应行
		string response_header;//响应报头
		string response_blank;//响应空行
		string response_body;//响应正文
	private:
		int fd;
		int size;
	public:
		HttpResponse()
			:response_blank("\n")
		{}
		void MakeResponseLine(int code)//构建响应行
		{
			string version="HTTP/1.1";
			response_line=version;
			response_line+=" ";
			response_line+=Util::IntToString(code);
			response_line+=" ";
			response_line+=Util::CodeToDec(code);
			response_line+="\r\n";
		}

		void MakeResponseHeader(vector<string>& v)//构建响应报头
		{
			auto it=v.begin();
			for(;it!=v.end();++it)
			{
				response_header+=*it;
				response_header+="\r\n";
			}
		}

		void MakeResponse(HttpRequest* rq,int code,bool cgi)//构建响应正文
		{
			MakeResponseLine(code);//构建响应行
			//构建响应报头
			vector<string> v;
			if(cgi)//说明是POST方法或者是GET带参数的方法
			{
				string ct=Util::SuffixToType("");
				string cl="Content-Length: ";
				cl+=Util::IntToString(response_body.size());
				v.push_back(ct);
				MakeResponseHeader(v);
			}
			else//一定是GET方法且不带参数
			{
				string suffix=rq->GetSuffix();//拿到后缀
				size=rq->GetResourceSize();
				//构建响应报头
				//写后缀
				string ct=Util::SuffixToType(suffix);
				v.push_back(ct);

				string cl="Content-Length: ";
				cl+=Util::IntToString(size);
				v.push_back(cl);
				MakeResponseHeader(v);
				//构建响应正文
				string path=rq->GetPath();
				cout<<"debug: "<<path<<endl;
				//获取到资源路径之后
				fd=open(path.c_str(),O_RDONLY);
			}
		}
		string& GetResponseLine()
		{
			return response_line;
		}
		string& GetResponseHeader()
		{
			return response_header;
		}
		string& GetResponseBlank()
		{
			return response_blank;
		}
		string& GetResponseBody()
		{
			return response_body;
		}
		int GetFd()
		{
			return fd;
		}
		int GetResourceSize()
		{
			return size;
		}
		~HttpResponse()
		{
			if(fd!=-1)
				close(fd);
		}
};
class EndPoint
{
	private:
		int sock;
	public:
		EndPoint(int sock_)
			:sock(sock_)
		{}
		int RecvLine(string& line)//读取一行放在line中
		{
			char c='X';
			while(c!='\n')
			{
				ssize_t s=recv(sock,&c,1,0);
				if(s>0)
				{
					if(c=='\r')
					{
						if(recv(sock,&c,1,MSG_PEEK)>0)//窥探标志位
						{
							if(c=='\n')//说明是\r\n的情况
							{
								recv(sock,&c,1,0);
							}
							else//说明是\r的情况
							{
								c='\n';
							}
						}
						else//读出错
						{
							c='\n';
						}
					}
					line.push_back(c);
				}
				else if(s==0)
				{
					c='\n';
				}
				else//读出错或者失败
				{
					c='\n';
					cout<<"recv error"<<s<<"sock: "<<sock<<endl;
				}
				line.push_back(c);
			}
			return line.size();//返回元素个数
		}
		void RecvRequestLine(HttpRequest* rq)//将请求行读到rq中
		{
			RecvLine(rq->GetRequestLine());
		}
		void RecvRequestHeader(HttpRequest* rq)//将请求报头读到rq中
		{
			string& rh=rq->GetRequestHeader();
			do
			{
				string line="";
				RecvLine(line);
				if(line=="\n")
				{
					break;
				}
				rh+=line;
			}while(1);
		}
		void RecvRequestBody(HttpRequest* rq)
		{
			int len=rq->GetContentLength();//获得长度
			string& body=rq->GetRequestBody();
			char c;
			while(len--)
			{
				if(recv(sock,&c,1,0)>0)
				{
					body.push_back(c);
				}
			}
			cout<<"body: "<<body<<endl;
		}
		
		//当获得了请求包含的各部分之后，开始响应
		void SendResponse(HttpResponse* rsp,bool cgi)
		{
				//不是cgi，一定是GET方法
			string& response_line=rsp->GetResponseLine();
			string response_header=rsp->GetResponseHeader();
			string& response_blank=rsp->GetResponseBlank();
			send(sock,response_line.c_str(),response_line.size(),0);
			send(sock,response_header.c_str(),response_header.size(),0);
			send(sock,response_blank.c_str(),response_blank.size(),0);
			if(cgi)//如果是POST方法或者GET有参数
			{
				string response_body=rsp->GetResponseBody();
				send(sock,response_body.c_str(),response_body.size(),0);
			}
			else
			{
				int fd=rsp->GetFd();
				int size=rsp->GetResourceSize();
			    sendfile(sock,fd,nullptr,size);//发送正文
			}
		}
		void ClearRequest(HttpRequest* rq)
		{
			if(rq->GetDone())//读完了
				return;
			if(rq->GetRequestHeader().empty())//如果请求头为空
			{
				RecvRequestHeader(rq);
			}
			if(rq->IsNeedRecv())
			{
				if((rq->GetRequestBody()).empty())//如果请求正文为空
					RecvRequestBody(rq);
			}
			rq->SetDone(true);//读完了
		}

		~EndPoint()
		{
			if(sock>=0)
				close(sock);
		}
};
//进行线程处理
class Entry
{
	public:
		static int ProcessCgi(HttpRequest* rq,HttpResponse* rsp)
		{
			int code=200;
			string path=rq->GetPath();
			string& body=rq->GetRequestBody();
			string& method=rq->GetMethod();
			string& query_string=rq->GetQueryString();
			int content_length=rq->GetContentLength();
			string cont_len_env="CONTENT_LENGTH= ";//设置环境变量
			string method_env="METHOD=";
			method_env+=method;
			string query_string_env="QUERY_STRING=";
			query_string_env+=query_string;
			cout<<"Query String"<<query_string_env<<endl;
			string& rsp_body=rsp->GetResponseBody();
			pid_t id=fork();
			int input[2]={0};
			int output[2]={0};
			pipe(input);
			pipe(output);//创建连个管道实现全双工通信
			if(id<0)
			{
				code=500;
			}
			else if(id==0)//child
			{
				close(input[1]);//要进行读
				close(output[0]);//要进行写
				dup2(input[0],0);//读到的内容重定向到标准输入
				dup2(output[1],1);//将写的内容重定向到标准输出
				putenv((char*)method_env.c_str());//将方法环境变量添加进来
				if(method=="POST")
				{
					cont_len_env+=Util::IntToString(content_length);
					putenv((char*)cont_len_env.c_str());//将Content_length作为环境变量设置进子进程
				}
				else if(method=="GET")
				{
					putenv((char*)query_string_env.c_str());
				}
				else
				{}
				execl(path.c_str(),path.c_str(),nullptr);
				exit(1);
			}
			else//father
			{
				close(input[0]);
				close(output[1]);
				auto it=body.begin();
				for(;it!=body.end();++it)
				{
					char c=*it;
					write(input[1],&c,1);
				}
				char c;
				while(read(output[0],&c,1)>0)
				{
					rsp_body.push_back(c);
				}
				waitpid(id,nullptr,0);
			}
			return code;
		}
		static void* HandlerRequest(void* arg)
		{
			int code=200;
			int* p=(int*)arg;
			int sock=*p;
			EndPoint* ep=new EndPoint(*(int*)arg);
			HttpRequest* rq=new HttpRequest();
			HttpResponse* rsp=new HttpResponse();
			ep->RecvRequestLine(rq);//获取请求行到rq中
			rq->RequestLineParse();//解析
			if(!rq->MethodIsLegal())
			{
				code=404;
				goto end;
			}
			ep->RecvRequestHeader(rq);//获取请求报头
			rq->RequestHeaderParse();//对报头进行解析
			if(!rq->IsNeedRecv())
			{
				ep->RecvRequestBody(rq);//读取正文
			}
			rq->SetDone(true);//读完
			rq->UrlParse();
			if(!rq->IsPathLegal())
			{
				goto end;
			}
			if(rq->IsCgi())
			{
				//CGI
				code=ProcessCgi(rq,rsp);
				if(code==200)
				{
					rsp->MakeResponse(rq,code,true);
					ep->SendResponse(rsp,true);
				}
			}
			else//不是CGI，一定不是POST方法
			{
				rsp->MakeResponse(rq,code,false);//执行响应
				ep->SendResponse(rsp,false);
			}
end:
			if(code!=200)
			{
				cout<<"error code..."<<code<<endl;
				ep->ClearRequest(rq);
				rq->ReMakeRequest(code);
				rsp->MakeResponse(rq,code,false);
				ep->SendResponse(rsp,false);
			}
			delete rq;
			delete rsp;
			delete ep;
			delete p;
		}
};
