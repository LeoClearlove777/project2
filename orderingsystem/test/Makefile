json:json.cpp
	g++ -std=c++11 $^ -o $@ -ljsoncpp
mysql:mysql.c
	gcc mysql.c -o mysql -L/usr/lib64/mysql -lmysqlclient
.PHONY:
clean:
	rm -f mysql
