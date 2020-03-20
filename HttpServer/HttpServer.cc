#include "HttpServer.hpp"
static void Usage(string proc)
{
	cout<<"Usage:"<<proc<<endl;
}
int main(int argc,char* argv[])
{
	if(argc!=2)
	{
		Usage(argv[0]);
		exit(1);
	}
	HttpServer* htp=new HttpServer(atoi(argv[1]));
	htp->InitHttpServer();
	htp->Start();
	delete htp;
	return 0;
}

