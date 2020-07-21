#include "client.h"


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

