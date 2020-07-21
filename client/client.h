
#ifndef _HEAD_H_
#define _HEAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include "cJSON.h"

//消息的类型
#define USER_REGISTER    10
#define USER_LOGIN       20
#define USER_WORD        30
#define USER_SUCCESS     40
#define USER_FAILURE     50

//云服务器ip和端口
#define IPADDR "47.97.218.49"
#define PORT   "8888"


//用户输入指令，供大家选择
enum{
	LOGIN    = 1,  //登陆
	REGISTER = 2,  //注册
	QUIT     = 3,  //退出
	QUERY    = 1,  //查询单词
	HISTORY  = 2,  //查询历史
    NETWORK  = 0,  //网络查询
};

//__attribute__((__packed__))
//作用：告诉编译器在编译的过程中取消优化对齐。
//      方便我们在发送和接收数据的时候一个字节一个字节的排列 
typedef struct
{
	char _username[25]; //用户名
	char _password[25]; //密码
}__attribute__((__packed__))user_t;

typedef struct
{
	int type;           //消息类型
	int size;           //消息大小
	union
	{
		user_t uinfo;	    //用户信息
		char   _word[100]; 
	}content;

//客户端填单词，服务端填写单词解释
#define word         content._word
#define username     content.uinfo._username
#define password       content.uinfo._password
}__attribute__((__packed__))mhead_t;


//'\'表示多行链接上一行表示， #deifne ....do...while(0);
//表示封装成独立的语法单元，防止被语法错误。
//注意：'\'之后不要留空格，要不然编译会有警告

#define EXEC_SQL(db,sql,errmsg) do{\
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) < 0)\
	{\
		fprintf(stderr,"sqlite3 execl [%s] error : %s.\n",sql,errmsg);\
		exit(EXIT_FAILURE);\
	}\
}while(0);


int baidui_api(char *q);
void help_info1();
void help_info2();
int init_tcp(char *ip,char *port);
int do_register(int sockfd);
int user_login(int sockfd);
int do_query(int sockfd);
int do_history();
size_t callback(void * buffer,size_t size,size_t nmemb,void* stream);
int baidu_api(char *q);
int main_widget(int sockfd);
int do_task(int sockfd);




#endif
