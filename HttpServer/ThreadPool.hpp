#pragma once
#include <iostream>
using namespace std;
#include <pthread.h>
#include <queue>
typedef void*(*handler_t)(void*);
class Task
{
	private:
		int* sock_p;
		handler_t handler;
	public:
		Task()
			:sock_p(nullptr)
			 ,handler(nullptr)
	{}
		Task(int* sock_p_,handler_t h_)
			:sock_p(sock_p_)
			 ,handler(h_)
	{}
		void Run()
		{
			handler(sock_p);
		}
		~Task()
		{}
};
class ThreadPool
{
	private:
		int num;//线程数
		queue<Task> q;//任务队列
		pthread_mutex_t lock;
		pthread_cond_t cond;
	public:
		ThreadPool(int _num=10)
			:num(_num)
		{
			pthread_mutex_init(&lock,nullptr);
			pthread_cond_init(&cond,nullptr);
		}
	private:
		void LockQueue()
		{
			pthread_mutex_lock(&lock);
		}
		void UnlockQueue()
		{
			pthread_mutex_unlock(&lock);
		}
		bool HasTask()
		{
			return q.size()==0 ? false:true;
		}
		void ThreadWait()
		{
			pthread_cond_wait(&cond,&lock);
		}
		void ThreadSignal()
		{
			pthread_cond_signal(&cond);
		}
	public:
		Task PopTask()
		{
			Task t=q.front();
			q.pop();
			return t;
		}
		void PushTask(const Task& t)
		{
			LockQueue();
			q.push(t);
			UnlockQueue();
			ThreadSignal();
		}
		void InitThreadPool()
		{
			pthread_t tid;
			for(int i=0;i<num;++i)
			{
				pthread_create(&tid,nullptr,ThreadRoutine,this);
			}
		}
	private:
		static void* ThreadRoutine(void* args)
		{
			//因为static的成员函数没有this指针，而是传入对象args相当于this指针，因此这里要定义线程池对象tp，用线程池对象来操作
			ThreadPool* tp=(ThreadPool*)args;
			while(1)
			{
				tp->LockQueue();
				while(!tp->HasTask())//如果没有任务
				{
					tp->ThreadWait();
				}
				Task t=tp->PopTask();
				tp->UnlockQueue();
				t.Run();
			}
		}
	public:
		~ThreadPool()
		{
			pthread_mutex_destroy(&lock);
			pthread_cond_destroy(&cond);
		}
};
