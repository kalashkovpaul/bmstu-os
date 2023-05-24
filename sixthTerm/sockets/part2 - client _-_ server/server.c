#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define SOCK_NAME "socket"
#define SOCK_NAME_CLIENT "socket2"
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

int main(void)
{
    struct sockaddr server, client;
    char buf[1024];
    int  bytes;

    if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() failed");
        return EXIT_FAILURE;
    }

    server.sa_family = AF_UNIX;
    strcpy(server.sa_data, SOCK_NAME);

    if (bind(fd, &server, sizeof(server)) == -1)
    {
        perror("bind failed");
        del_socket();
        return EXIT_FAILURE;
    }

    signal(SIGTSTP, sig_handler); //изменение обработчика сигнала

    printf("Waiting for messages.\nCtrl + Z to stop...\n\n");

    while (1)
    {
        bytes = recvfrom(fd, buf, 1024, 0, NULL, NULL);
        if (bytes < 0)
        {
            del_socket();
            printf("recvfrom() failed");
            return EXIT_FAILURE;
        }
        buf[bytes] = 0;
        printf("Server recieved: %s\n", buf);
    }

    del_socket();
    return EXIT_SUCCESS;
}   
