#include "server.h"


struct sockaddr_in svr_addr;
sqlite3 *db;
int sfd;


void user_register(int cfd,char *username,char *password)
{
    //将username和password插入数据库
    char *errmsg;
    char **dbresult;
    int nrow=0,ncolumn=0;
    char sql[1024]={0};
    char s_buf[1024]={0};
    mhead_t *head=(mhead_t *)s_buf;
    //先查询username是否存在数据库
    sprintf(sql,"select * from user_table where NAME=\'%s\';",username);//先拼接出sql语句
    if(sqlite3_get_table(db,sql,&dbresult,&nrow,&ncolumn,&errmsg)!=0)
    {
        fprintf(stderr,"sqlite3_get_table:%s",errmsg);
        exit(EXIT_FAILURE);
    }
    if(nrow==0)//如果没有查询到内容，就说明该用户没有注册过，可以进行插入
    {
        bzero(sql,sizeof(sql));
        sprintf(sql,"insert into user_table values(\'%s\',\'%s\');",username,password);
        EXEC_SQL(db,sql,errmsg)
        
        printf("insert success~\n");
        
        //封装一个数据包给客户端
        head->type=USER_SUCCESS;
        if(send(cfd,s_buf,sizeof(mhead_t),0)<0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }

    }
    else   //用户名已经存在，注册失败
    {
        head->type=USER_FAILURE;
        if(send(cfd,s_buf,sizeof(mhead_t),0)<0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }

    }
}

void user_login(int cfd,char *username,char *password)
{
    //用户登陆，用用户输入的username去查寻密码，再和用户输入的密码是否匹配i
    char *errmsg;
	char s_buf[1024];
	char **dbresult;
	int nrow = 0,ncolumn = 0;
	char sql[1024] = {0};
	mhead_t *head = (mhead_t *)s_buf;	

    sprintf(sql,"select * from user_table where NAME=\'%s\' AND PASSWORD=\'%s\';",username,password);
    if(sqlite3_get_table(db,sql,&dbresult,&nrow,&ncolumn,&errmsg)!=0)
    {
        fprintf(stderr,"sqlite3_get_table:%s\n",errmsg);
        exit(EXIT_FAILURE);
    }
    if(nrow==0)
    {
        //没有查询到该用户，用户名或密码错误，请先注册用户
        head->type=USER_FAILURE;
        if(send(cfd,s_buf,sizeof(mhead_t),0)<0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        //如果查到了内容，就说明用户名密码都是对的
        bzero(s_buf,sizeof(s_buf));
        head->type=USER_SUCCESS;
        if(send(cfd,s_buf,sizeof(mhead_t),0)<0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }
}


int user_query(int cfd,char *word)
{
    //去数据库里查单词，并且把查到的单词发回客户端
    //printf("in user_query\n");
    char sql[128]={0};
    char *errmsg;
    char **dbresult;
    int nrow,ncolumn;
    char buf[1024]={0};
    
    mhead_t *head=(mhead_t *)buf;
    bzero(sql,sizeof(sql));
    sprintf(sql,"select explain from dict_table where word=\'%s\'; ",word);
    printf("%s\n",sql);
    if(sqlite3_get_table(db,sql,&dbresult,&nrow,&ncolumn,&errmsg)!=0)
    {
        fprintf(stderr,"sqlite3_get_table:%s\n",errmsg);
        exit(EXIT_FAILURE);
    }
    printf("flag\n");
    if(nrow==0)    //如果nrwo==0就说明没有查到这个单词，要给客户端发送错误
    {
        bzero(buf,sizeof(buf));
        head->type=USER_FAILURE;
        //strcpy(head->content._word,"THE WORD IS NOT IN THE DATABASE!\n");
        if(send(cfd,buf,sizeof(mhead_t),0)<0)
        {
            printf("fal\n");
            perror("send");
            exit(EXIT_FAILURE);
        }
        printf("nofound\n");
    }
    else
    {
        printf("else\n");
    bzero(buf,sizeof(buf));
    head->type=USER_WORD;
    
    strcpy(head->content._word,dbresult[1]);
    
    if(send(cfd,buf,sizeof(mhead_t),0)<0)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("send:%s\n",head->content._word);
    printf("send success~\n");
    return 0;
    }
}
void recv_msg(int sfd)
{

    //创建epoll模型
    int epfd=epoll_create(1);  //拿到epoll的描述符
    
    //将文件描述符添加到红黑树中
    struct epoll_event ev;
    ev.data.fd=sfd;
    ev.events=EPOLLIN;
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&ev)<0)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
    
    int ret,i,cfd,count;
    struct sockaddr_in cli_addr;
    socklen_t len=sizeof(cli_addr);
    struct epoll_event new_ev[10];
    char r_buf[1024];
    mhead_t *head = (mhead_t *)r_buf;
    bzero(head,sizeof(mhead_t));
    ssize_t sbytes;
    while(1)
    {
        ret=epoll_wait(epfd,new_ev,10,500);   //超时时间500毫秒
    //    printf("ret:%d\n",ret);
        if(ret<0)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        else if (ret==0)
        {
            printf("wait...\n");
        }
        else if (ret>0)   //表示有套接字已经准备就绪
        {
            for(i=0;i<ret;i++)
            {
                printf("fd:%d\n",new_ev[i].data.fd);
                if(new_ev[i].data.fd==sfd)
                {
                
                    cfd = accept(sfd,(struct sockaddr *)&cli_addr,&len);
                    if(cfd<0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    printf("ip:%s  port:%d\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));
                    
                    //做一个ev结构体来存新的发送套接字
                    struct epoll_event cli_ev;
                    cli_ev.data.fd=cfd;
                    cli_ev.events=EPOLLIN;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&cli_ev);    //把新得到的cfd加入红黑树里
                }
                else
                {

                    //如果当前遍历得到的套接字不是用于连接的，那么就是用于消息发送接受的
                    count=0;
                    //保证接收完整的结构体
                    //printf("before while\n");
                    while(1)
                    {
                        sbytes=recv(new_ev[i].data.fd,r_buf+count,sizeof(mhead_t)-count,0);
                        if(sbytes<0)
                        {
                            perror("recv");


                            exit(EXIT_FAILURE);
                        }
                        if(sbytes==0)
                        {
                            printf("client is offline!\n");
                            epoll_ctl(epfd,EPOLL_CTL_DEL,new_ev[i].data.fd,NULL);
                            close(new_ev[i].data.fd);
                            bzero(head,sizeof(mhead_t));
                            break;
                        }
                        count+=sbytes;     
                        if(count==sizeof(mhead_t))   //当实际接受了1024字节时，就完整接受了一个结构体
                        {
                            break;
                        }
                    }
                   //printf("last \n");
                    //这时候我们要对结构体解包分析，看用户发送的是哪种信息
                    switch(head->type)
                    {
                        case USER_REGISTER:
                        //用户注册
                        user_register(new_ev[i].data.fd,head->content.uinfo._username,head->content.uinfo._password);
                        break;
                        case USER_LOGIN:
                        //用户登陆
                        user_login(new_ev[i].data.fd,head->content.uinfo._username,head->content.uinfo._password);
                        break;
                        case USER_WORD:
                        //用户查询单词
                        user_query(new_ev[i].data.fd,head->content._word);
                        break;
                    }
                }
            }
        }
    }


}



int  socket_init(char ip[],char port[])
{
   int sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    bzero(&svr_addr,sizeof(svr_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port   = htons(atoi(port));     //大小端转化  htos:host order-->net order atoi: string-->int  
    svr_addr.sin_addr.s_addr   = inet_addr(ip); 
     if(bind(sfd,(struct sockaddr *)&svr_addr,sizeof(svr_addr))<0)
    {
        perror("bind()");
        exit(EXIT_FAILURE); 
    }
    printf("bind success~\n");
    
    if(listen(sfd,1)<0)
    {
        perror("listen()");
        return 0;
    }

    return sfd;
}

#if 0
int main(int argc,char *argv[])
{
    if(argc<3)
    {
        printf("argc too short!\n");
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
#endif    

