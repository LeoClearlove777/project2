#pragma once
#include <iostream>
using namespace std;
#include "Protocol.hpp"
#include "Util.hpp"
#include "ThreadPool.hpp"//使用线程池
class Sock//将套接字的操作进行封装
{
	private:
		int sock;
		int port;
	public:
		Sock(const int& _port)
			:port(_port)
			 ,sock(-1)
	{}
		void Socket()
		{
			sock=socket(AF_INET,SOCK_STREAM,0);
			if(sock<0)
			{
				cerr<<"socket error"<<endl;
				exit(2);
			}
			int opt=1;
			setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
		}

		void Bind()
		{
			struct sockaddr_in local;//绑定源端地址信息和ip地址及端口号
			local.sin_family=AF_INET;
			local.sin_port=htons(port);
			local.sin_addr.s_addr=htonl(INADDR_ANY);
			if(bind(sock,(struct sockaddr*)& local,sizeof(local))<0)
			{
				cerr<<"bind error"<<endl;
				exit(3);
			}
		}

		void Listen()
		{
			const int backlog=10;
			if(listen(sock,backlog)<0)
			{
				cerr<<"listen error"<<endl;
				exit(4);
			}
		}
		int Accept()
		{
			//接收请求是接收客户端连接请求
			struct sockaddr_in peer;
			socklen_t len=sizeof(peer);
			int fd=accept(sock,(struct sockaddr*)& peer,&len);
			if(fd<0)
			{
				cerr<<"accept error"<<endl;
				return -1;
			}
			cout<<"get a new link...done"<<endl;
			return fd;
		}
		~Sock()
		{
			if(sock>=0)
			{
				close(sock);
			}
		}
};
#define DEFAULT_PORT 8080
class HttpServer
{
	private:
		Sock sock;
		ThreadPool* tp;//线程池对象
	public:
		HttpServer(int port=DEFAULT_PORT)
			:sock(port)
			 ,tp(nullptr)
		{}
		void InitHttpServer()
		{
			sock.Socket();//创建套接字
			sock.Bind();//绑定
			sock.Listen();//监听
			tp=new ThreadPool(8);//放8个线程
			tp->InitThreadPool();//线程池初始化
		}
		void Start()
		{
			//接收请求是循环的
			for(;;)
			{
				int sock_=sock.Accept();
				if(sock_>=0)//接收新的套接字成功
				{
					//线程操作
					pthread_t tid;
					int* p=new int(sock_);
					Task t(p,Entry::HandlerRequest);//创建一个任务，任务的动作在Entry类中实现
					tp->PushTask(t);//将任务放入任务队列
				}
			}
		}
};
