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
#include <sys/epoll.h>
#include <unistd.h>
#include <stdbool.h>

//消息的类型
#define USER_REGISTER  	 10 
#define USER_LOGIN    	 20 
#define USER_WORD      	 30 
#define USER_SUCCESS     40 
#define USER_FAILURE     50


#if 0
typedef struct 
{
	char _username[25];
	char _password[25];
} __attribute__ ((__packed__))user_t;
#endif

typedef struct 
{
	char _username[25];
	char _password[25];
}user_t;

typedef struct
{
	int type;
	int size;
	union 
	{
		user_t uinfo;
		char  _word[100];
	}content;
//客户端填单词，服务器端填单词解释
//#define word 		content._word 
//#define username	content.uinfo._username
//#define password  	content.uinfo._password
}mhead_t;

#define EXEC_SQL(db,sql,errmsg) do{\
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) < 0)\
	{\
		fprintf(stderr,"sqlite3 execl [%s] error : %s.\n",sql,errmsg);\
		exit(EXIT_FAILURE);\
	}\
}while(0);


void user_register(int cfd,char *username,char *password);
void user_login(int cfd,char *username,char *password);
int user_query(int cfd,char *word);
void recv_msg(int sfd);
int  socket_init(char ip[],char port[]);




#endif
