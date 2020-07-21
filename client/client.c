#include "client.h"


int baidui_api(char *q);

//用户提示界面1
void help_info1()
{
	printf("\t-----------------------------------------------\n");
	printf("\t|               HQYJ   在线辞典                |\n");
	printf("\t|版本:0.0.1                                    |\n");
	printf("\t|作者:lc                                       |\n");
	printf("\t|功能:                                         |\n");
	printf("\t|    [1] 登录                                  |\n");
	printf("\t|    [2] 注册                                  |\n");
	printf("\t|    [3] 退出                                  |\n");
	printf("\t|注意:用户只有登录成功后才能进入查单词界面     |\n");
	printf("\t------------------------------------------------\n");
	return;
}

//用户提示界面2
void help_info2()
{
	printf("\n\t-----------------------------------------------\n");
	printf("\t|               HQYJ   在线辞典                |\n");
	printf("\t|版本:0.0.1                                    |\n");
	printf("\t|作者:lc                                       |\n");
	printf("\t|功能:                                         |\n");
	printf("\t|    [0] 网络查词                              |\n");
	printf("\t|    [1] 查询单词                              |\n");
	printf("\t|    [2] 查询历史                              |\n");
	printf("\t|    [3] 退出                                  |\n");
	printf("\t|注意:用户只有登录成功后才能进入查单词界面     |\n");
	printf("\t------------------------------------------------\n");
	return;
}

int init_tcp(char *ip,char *port)
{
	int sockfd;
	struct sockaddr_in server_addr;

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("Fail to socket");	
		exit(EXIT_FAILURE);
	}

	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(ip);

	if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
	{
		perror("Fail to bind");	
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

int do_register(int sockfd)
{
	int n = 0;
	int count = 0;
	char buf[1024] = {0};
	//定义发送的协议头
	mhead_t *head = (mhead_t *)buf;

	printf("\n您正在注册，请输入用户名和密码\n");

	head->type = USER_REGISTER;
	head->size = sizeof(mhead_t);

	printf("Input username : ");
	fgets(head->username,sizeof(head->username),stdin);
	head->username[strlen(head->username) - 1] = '\0';

	printf("Input password : ");
	fgets(head->password,sizeof(head->password),stdin);
	head->password[strlen(head->password) - 1] = '\0';

	//发给服务器端
	
	if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
	{
		perror("Fail to send");
		exit(EXIT_FAILURE);
	}
	
	bzero(&buf,sizeof(buf));
	while(1)
	{
		//接收数据，TCP是可靠的连接，若是数据
		//未完全接收的话，可以在接收
		n = recv(sockfd,buf + count,sizeof(mhead_t) - count,0);

		if(n <= 0){
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}
		//若是数据未发送完成，再次接收的时候可补充
		count += n;
		if(count == sizeof(mhead_t))
			break;
	}

	if(head->type == USER_SUCCESS)
	{
		printf("\n恭喜您，注册成功!\n");	
		return 0;
	}else{
		printf("\n很遗憾，这个用户名已经被其它用户注册过了，请重新注册");	
		return -1;
	}

}



int user_login(int sockfd)
{

	int n = 0;
	int count = 0;
	char buf[1024] = {0};
	//定义发送的协议头
	mhead_t *head = (mhead_t *)buf;

	printf("\n您正在登陆，请输入用户名和密码\n");

	head->type = USER_LOGIN;
	head->size = sizeof(mhead_t);

	printf("Input username : ");
	fgets(head->username,sizeof(head->username),stdin);
	head->username[strlen(head->username) - 1] = '\0';

	printf("Input password : ");
	fgets(head->password,sizeof(head->password),stdin);
	head->password[strlen(head->password) - 1] = '\0';

	//发给服务器端
	
	if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
	{
		perror("Fail to send");
		exit(EXIT_FAILURE);
	}
	
	bzero(&buf,sizeof(buf));
	while(1)
	{
		//接收数据，TCP是可靠的连接，若是数据
		//未完全接收的话，可以在接收
		n = recv(sockfd,buf + count,sizeof(mhead_t) - count,0);

		if(n <= 0){
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}
		//若是数据未发送完成，再次接收的时候可补充
		count += n;
		if(count == sizeof(mhead_t))
			break;
	}

	if(head->type == USER_SUCCESS)
	{
		printf("\n恭喜您,登陆成功\n");	
		return 0;
	}else{
		printf("\n登陆失败，用户名或密码错误\n");

		return -1;
	}

}

int do_query(int sockfd)
{
    char buf[1024];
    char send_word[100];
    int count=0,n=0;
    FILE *fp=fopen("./.history.txt","a+");
    mhead_t *head=(mhead_t *)buf;
    char msg[1024];
    printf("\nplease input your word:\n ");
    fgets(send_word,sizeof(send_word),stdin);
    send_word[strlen(send_word)-1]='\0';
    //将单词封包发送给服务端
    head->type=USER_WORD;
    strcpy(head->content._word,send_word);
    if(send(sockfd,buf,sizeof(mhead_t),0)<0)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    //printf("send word success~\n");
    bzero(buf,sizeof(buf));
    while(1)
    {
        //接收数据，TCP是可靠的连接，若是数据
        //未完全接收的话，可以在接收
        n = recv(sockfd,buf + count,sizeof(mhead_t) - count,0);

        if(n <= 0){
            perror("Fail to recv");
            exit(EXIT_FAILURE);
        }
        //若是数据未发送完成，再次接收的时候可补充
        count += n;
        if(count == sizeof(mhead_t))
            break;
    }
    if(head->type==USER_WORD)
    {
        printf("\n%s\n",head->content._word);
        bzero(msg,sizeof(msg));
        sprintf(msg," %s : %s\n",send_word,head->content._word);
        fputs(msg,fp);
        fflush(fp);
        fclose(fp);
    }
    else if(head->type==USER_FAILURE)
    {
        printf("数据库中没有该单词，已为您智能翻译:\n");
        baidu_api(send_word);
    }
}


int do_history()
{
    FILE *fp;
    char history[1024];
    fp=fopen("./.history.txt","r");
    while(fgets(history,sizeof(history),fp)!=NULL)
    {
        fputs(history,stdout);
    }
}



size_t callback(void * buffer,size_t size,size_t nmemb,void* stream)
{
    //printf("callback\n");
    FILE *fp=(FILE*)stream;
    fwrite(buffer,size,nmemb,fp);
    FILE *history_fp=fopen("./.history.txt","a+");
    char result[1024];
    if(history_fp==NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    char *buf=(char *)buffer;
    //printf("%s\n",buf);
    cJSON *cjson=cJSON_Parse(buf);
    if(cjson==NULL)
    {
        printf("cjson error!\n");
    }
    cJSON *arr=cJSON_GetObjectItem(cjson,"trans_result");
    cJSON *trans=cJSON_GetArrayItem(arr,0);
    cJSON *querry=cJSON_GetObjectItem(trans,"src");
    cJSON *item=cJSON_GetObjectItem(trans,"dst");
    //cJSON_Print(item);
    if(item==NULL)
    {
        printf("process json error!\n");
        return -1;
    }
    else{
        //printf("process json success!\n");
        //printf("%s\n",querry->valuestring);
        //printf("%s\n",item->valuestring);
        bzero(result,sizeof(result));
        sprintf(result," %s : %s\n",querry->valuestring,item->valuestring);
        printf("%s\n",result);
        fputs(result,history_fp);
        fflush(history_fp);
        fclose(history_fp);
    }
    cJSON_Delete(cjson);
    return size*nmemb;

}

int baidu_api(char *q)
{
  CURL *curl;
  CURLcode res;
  FILE *fp;
  fp=fopen("./.json.txt","w+");
  if(fp==NULL)
  {
      perror("fopen");
      return -1;
  }
  curl = curl_easy_init();
  if(curl) {
    char myurl[1000] = "http://api.fanyi.baidu.com/api/trans/vip/translate?";
    char *appid = "20200714000518868";    //replace myAppid with your own appid
    //char *q = "apple";          //replace apple with your own text to be translate, ensure that the input text is encoded with UTF-8!
    char *from = "en";          //replace en with your own language type of input text
    char *to = "zh";            //replace zh with your own language type of output text
    char salt[60];
    int a = rand();
    sprintf(salt,"%d",a);
    char *secret_key = "bbb8RNzlFZW6A401o5v8";   //replace mySecretKey with your own mySecretKey
    char sign[120] = "";
    strcat(sign,appid);
    strcat(sign,q);
    strcat(sign,salt);
    strcat(sign,secret_key);
    //printf("%s\n",sign);
    unsigned char md[16];
    int i;
    char tmp[3]={'\0'},buf[33]={'\0'};
    MD5((unsigned char *)sign,strlen((unsigned char *)sign),md);
    for (i = 0; i < 16; i++){
        sprintf(tmp,"%2.2x",md[i]);
        strcat(buf,tmp);
    }
    //printf("%s\n",buf);
    strcat(myurl,"appid=");
    strcat(myurl,appid);
    strcat(myurl,"&q=");
    strcat(myurl,q);
    strcat(myurl,"&from=");
    strcat(myurl,from);
    strcat(myurl,"&to=");
    strcat(myurl,to);
    strcat(myurl,"&salt=");
    strcat(myurl,salt);
    strcat(myurl,"&sign=");
    strcat(myurl,buf);
    //printf("%s\n",myurl);
    //���÷��ʵĵ�ַ
    curl_easy_setopt(curl, CURLOPT_URL, myurl);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,callback);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  return 0;
}


int main_widget(int sockfd)
{
    //显示帮助界面2
    while(1)
    {
        help_info2();  
        int cmd;
        char q[128];
        printf("\n\n请选择>\n");
        scanf("%d",&cmd);
        getchar();  //接收回车 
        switch(cmd)
        {
        case NETWORK:
            printf("please input your word!\n");
            fgets(q,sizeof(q),stdin);
            q[strlen(q)-1]='\0';
            baidu_api(q);
            break;
        case QUERY:
            do_query(sockfd);
            break;
        case HISTORY:
            do_history();
            break;
        case QUIT:
            exit(EXIT_FAILURE);
            break;
        }
    }
}


int do_task(int sockfd)
{
	int cmd;
	while(1)
	{
		//提示界面帮助，用户选择
		help_info1();	

		printf("\n\n请选择>");
		scanf("%d",&cmd);
		//吃掉回车键
		getchar();
		switch(cmd)
		{
			//用户注册，我们先来写注册的函数
			case REGISTER:
				if(do_register(sockfd) < 0)
					continue;
					
			//用户登陆
			case LOGIN:	
					 if(user_login(sockfd)<0)
                     continue;
                     else{
                        main_widget(sockfd);
                        break;
                     }
			case QUIT:
				exit(EXIT_SUCCESS);
			default:
				printf("Unknow cmd.\n");
				continue;
		}
	}
	return 0;
}


//./client ip port 
//由于后面要传递参数，故这里的const省略
#if 0
int main(int argc, char *argv[])
{
	int sockfd;	
	int addr_len = sizeof(struct sockaddr);
	struct sockaddr_in peer_addr;

	if(argc < 3)
	{
		sockfd = init_tcp(IPADDR,PORT);
	}
    else
    {
	    sockfd = init_tcp(argv[1],argv[2]);
    }
	do_task(sockfd);

	return 0;
}
#endif

