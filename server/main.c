#include "server.h"


struct sockaddr_in svr_addr;
sqlite3 *db;
int sfd;


int main(int argc,char *argv[])
{
    if(argc<3)
    {
        printf("未指定IP和端口\n");
        return 0;
    }
    
    sfd=socket_init(argv[1],argv[2]); 
    //打开数据库
    if(sqlite3_open("./system.db",&db)<0)
    {
        fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    recv_msg(sfd);
    
    return 0;
}   

