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
#define N_CLIENTS 5

int fd;

void del_socket(void)
{
    if (close(fd) == -1)
    {
        printf("close() failed");
        return;
    }
    if (unlink(SOCK_NAME) == -1)
    {
        printf("unlink() returned -1");
    }
}

void sig_handler(int signum)
{
	printf("Catch SIGTSTP\n");
    del_socket();
    exit(0);
}

int main()
{
	int client_sock_fd;

	signal(SIGTSTP, sig_handler);

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd == -1)
	{
		printf("socket() failed\n");
		return EXIT_FAILURE;
	}

	struct sockaddr sock_addr;
	sock_addr.sa_family = AF_UNIX;
	strcpy(sock_addr.sa_data, SOCK_NAME);

	if(bind(fd, &sock_addr, sizeof(sock_addr)) == -1)
	{
        printf("bind() failed\n");
        del_socket();
        return EXIT_FAILURE;
	}

	if(listen(fd, N_CLIENTS) == -1 )
	{
        printf("listen() failed\n");
        del_socket();
        return EXIT_FAILURE;
	}

	printf("Listening.\nCtrl + Z to stop...\n\n");

	int bytes;
	char recv_buf[BUF_SIZE];
	char send_buf[BUF_SIZE];

    unsigned int sock_len;
    struct sockaddr remote_sock_addr;

    while(1)
    {
        if((client_sock_fd = accept(fd, NULL, NULL)) == -1 )
        {
            printf("Error on accept() call \n");
            return 1;
        }
        if (fork() == 0) {
            int new_fd = client_sock_fd;
            memset(recv_buf, 0, BUF_SIZE);
            memset(send_buf, 0, BUF_SIZE);
            while (read(new_fd, recv_buf, BUF_SIZE))
            {
                printf("Server received: %s\n", recv_buf);
                strcpy(send_buf, "Received;\n");

                if(write(new_fd, send_buf, strlen(send_buf)+1) == -1 )
                    printf("write() failed\n");
                printf("Server answering: %s\n", send_buf);
            }
            close(new_fd);
            exit(0);
        }

    }


    close(client_sock_fd);
	del_socket();
	return 0;
}