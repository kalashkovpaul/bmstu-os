#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define SOCK_NAME "socket"
#define BUF_SIZE 1024

int fd;

void sig_handler(int signum)
{
	printf("Catch SIGTSTP\n");
    close(fd);
    exit(0);
}


int main()
{

	int bytes;
	struct sockaddr_un server;
	char recv_msg[BUF_SIZE];
	char send_msg[BUF_SIZE];

	memset(recv_msg, 0, BUF_SIZE);
	memset(send_msg, 0, BUF_SIZE);
	signal(SIGTSTP, sig_handler);

	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, SOCK_NAME);

	sprintf(send_msg, "From pid %d", getpid());

	while (1)
	{
		if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 )
		{
			printf("socket() failed\n");
			return EXIT_FAILURE;
		}


		if(connect(fd, (struct sockaddr*)&server, strlen(server.sun_path) + sizeof(server.sun_family)) == -1)
		{
			printf("connect() failed\n");
			close(fd);
			return EXIT_FAILURE;
		}


		if(send(fd, send_msg, strlen(send_msg), 0 ) == -1)
		{
			printf("send() failed\n");
		}

		printf ("Child sent: %s\n", send_msg);
		memset(recv_msg, 0, BUF_SIZE);

		if((bytes = recv(fd, recv_msg, BUF_SIZE, 0)) > 0 )
		{
			printf("Client received: %s\n", recv_msg);
		}
		else
		{
			if(bytes < 0)
			{
				printf("recv() failed\n");
			}
			else
			{
				printf("Server socket closed \n");
				close(fd);
				break;
			}

		}
		sleep(3);
        printf("\n");
		close(fd);
	}

	return 0;
}