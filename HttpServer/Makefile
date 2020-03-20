.PHONY:all
all:HttpServer testCgi
testCgi:testCgi.cc
	g++ -o $@ $^ -std=c++11
HttpServer:HttpServer.cc
	g++ -o $@ $^ -lpthread -std=c++11
.PHONY:clean
clean:
	rm -f HttpServer testCgi
