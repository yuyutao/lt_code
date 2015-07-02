#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define FROM_FILE		'f'
#define HEX_STREAM		'h'
#define STRING			's'
#define MAX_SIZE		1024*10

int start_accept(int port) ;

int start_connect(const char* ip, int port) ;


static int m_sock_fd  = 0 ;
static int m_exit ;

static void* recv_thread(void* arg)
{
	char recv_buf[MAX_SIZE] ;
	int i = 0 ;
	while(!m_exit)
	{
		printf("#");
		fflush(stdout) ;
		memset(recv_buf,0,MAX_SIZE) ;
		int recv_len = recv(m_sock_fd, recv_buf, MAX_SIZE, 0) ;
		if(recv_len <=0)
		{
			printf("socket disconnect\n") ;
			exit(0);
		}
		for(i=0;i < recv_len ; ++i)
		{
			if(i % 10 == 0){
				printf("\n") ;
			}
			printf("%02x ",recv_buf[i]) ;
		}
		printf("\n");

		for(i=0;i < recv_len ; ++i)
		{
			printf("%c",recv_buf[i]) ;
		}
		printf("\n");
	}
}
static void send_file(char* file)
{
	FILE* fp = fopen(file,"rb") ;
	int len  ;
	fseek(fp, 0, SEEK_END) ;
	len = ftell(fp) ;
	fseek(fp, 0, SEEK_SET) ;
	void* buf = malloc(len) ;
	fread(buf, len, 1, fp) ;
	fclose(fp);
	send(m_sock_fd, buf, len, 0) ;
	free(buf) ;
}
static void send_str(char* str)
{
	send(m_sock_fd, str, strlen(str)+1, 0) ;
}
static void send_hex(char* hex_str)
{
	void* buf = malloc(MAX_SIZE) ;
	memset(buf,0,MAX_SIZE) ;
	int i = 0 ;
	char* s = strtok(hex_str," ") ;
	while(s != NULL)
	{
		char c = strtol(s,NULL,16) ;
		memcpy(buf+i,&c,1) ;
		i++;
		s = strtok(NULL, " ") ;
	}
	send(m_sock_fd,buf,i,0);
	free(buf);

}
int main(int argc, char* argv[])
{
	int port = 0 ;
	switch(argc){
		case 1 :
			printf("error no input\n");
			break ;
		case 2 :
			port = atoi(argv[1]) ;
			m_sock_fd = start_accept(port) ; 
			break ;
		case 3:
			port = atoi(argv[2]) ;
			m_sock_fd = start_connect(argv[1], port) ;
			break ;
		default :
			break ;

	}
	
	if(m_sock_fd <= 0)
	{
		printf("error sock \n") ;	
		return 0 ;
	}
	pthread_t thr ;
	pthread_create(&thr, NULL, recv_thread, NULL ) ;

	int tty_fd = open("/dev/tty", O_RDONLY, 0) ;
	if(tty_fd <=0 )
	{
		perror("tty open error") ;
		return -1 ;
	}
	
	char s[MAX_SIZE] = {0} ;

	const char* delim = " " ;
	while(1)
	{
		printf("#") ;
		fflush(stdout) ;
		memset(s,0,MAX_SIZE) ;
		int read_size = read(tty_fd, s, sizeof(s)) ;
		
		char *tok_s = strtok(s, delim) ; 
		if( tok_s == NULL || strlen(tok_s) != 1)
		{
			if(strcmp(tok_s,"q\n") == 0){
				break ;	
			}
			printf("error tty %s----\n",tok_s) ;
			continue ;
		}
		switch(tok_s[0])
		{
			case FROM_FILE :
				send_file(strtok(NULL,"")) ;	
				break ;
			case STRING :
				send_str(strtok(NULL,"")) ;
				break ;
			case HEX_STREAM :
				send_hex(strtok(NULL,"")) ;
				break ;
			default:
				printf("no support :%d\n",tok_s[0]) ;
				break;
		}

	}
	close(m_sock_fd) ;

}
