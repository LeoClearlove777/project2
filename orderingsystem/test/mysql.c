/*
 通过mysql的C语言接口实现
 1、初始化句柄mysql_init
 2、连接服务器mysql_real_connect
 3、设置字符集mysql_set_character_set
 4、执行语句mysql_query
 5、获取结果mysql_store_result
 6、遍历结果mysql_num_rows/mysql_num_fields/mysql_fetch_row
 7、关闭数据库mysql_close
 执行每个函数过程中获取上一次错误原因mysql_error
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

int main()
{
	MYSQL* mysql=mysql_init(NULL);//定义并初始化mysql操作句柄
	//mysql_real_connect(句柄,主机地址,用户名,密码,数据库名称,端口,套接字文件,标志),连接mysql服务器
	if(mysql_real_connect(mysql,"127.0.0.1","root","123456","db_order_sys",0,NULL,0) == NULL)
	{
		printf("mysql connect failed:%s\n",mysql_error(mysql));
		return -1;
	}
	//我们这里就不需要设置数据库了，我们上面已经连接了指定数据库
	int ret = mysql_set_character_set(mysql,"utf8");//设置客户端字符编码集
	if(ret!=0)
	{
		printf("set character to utf8 failed:%s\n",mysql_error(mysql));
		return -1;
	}
	
	char* sql_str="select * from tb_dishes";//定义一个查询语句
	ret=mysql_query(mysql,sql_str);//执行语句
	if(ret!=0)//执行失败
	{
		printf("sql query failed:%s\n",mysql_error(mysql));
		return -1;
	}

	MYSQL_RES* res=mysql_store_result(mysql);//获取执行结果
	if(res==NULL)
	{
		printf("store result failed:%s\n",mysql_error(mysql));
		return -1;
	}

	int row_nums=mysql_num_rows(res);//获取结果条数
	int col_nums=mysql_num_fields(res);//获取结果列数
	int i=0,j=0;
	for(;i<row_nums;++i)
	{
		MYSQL_ROW row=mysql_fetch_row(res);//遍历获取每一行的数据
		for(j=0;j<col_nums;++j)
		{
			printf("%s ",row[j]);//遍历获取每一列的数据
		}
		printf("\n");
	}
	mysql_free_result(res);//释放结果集
	mysql_close(mysql);//关闭数据库
	return 0;
}
