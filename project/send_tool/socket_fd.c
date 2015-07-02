
#include <arpa/inet.h>

#include <fcntl.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <string.h>

#include <stdio.h>

#include <netinet/in.h>

#include <stdlib.h>

#include <errno.h>

#include <sys/wait.h>

#include <netdb.h>

#include <sys/ioctl.h>

#include <sys/wait.h>

#include <semaphore.h>

#include <sys/time.h>

#include <signal.h>

#include <unistd.h>

#include <pthread.h>

#include <stdarg.h>


static struct sockaddr_in m_addr ;
static int server_sock  =0 ;
int start_accept(int port)
{
	if(server_sock == 0)
	{
		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons(port);
		m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_sock = socket(AF_INET, SOCK_STREAM, 0) ;
		if(server_sock == -1)
		{
			perror("socket error");
			return -1 ;
		}
		const int on = 1;   
		if(setsockopt(server_sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))     
		{
		}  
		if (bind(server_sock, (struct sockaddr *) &m_addr, sizeof(struct sockaddr))
						== -1) 
		{
			perror("bind error");
			return -1 ;
		}
		if (listen(server_sock, 10) == -1) 
		{
			perror("listen error");
			return -1;
		}
	}
	int client_fd ;
	socklen_t addr_size = 0 ;
	if((client_fd = accept(server_sock,(struct sockaddr*)&m_addr,&addr_size))== -1)
	{
		perror("accept error");	
	}
	return client_fd ;
}
int start_connect(const char* ip, int port)
{
	int client_fd ;
	if( (client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket error\n") ;
		return -1 ;
	}
	struct sockaddr_in addr ;
	addr.sin_family = AF_INET ;
	addr.sin_port = htons(port) ;
	inet_aton(ip, &addr.sin_addr) ;
	if(connect(client_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1 )
	{
		perror("connect error\n") ;
		return -1 ;
	}
	return client_fd ;

}
