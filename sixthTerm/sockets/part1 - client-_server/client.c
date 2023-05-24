#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SOCK_NAME "mysocket.soc"
#define BUF_SIZE 256

int main(void)
{
	int fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (fd == -1)
    {
        printf("socket failed");
        return EXIT_FAILURE;
    }

    struct sockaddr addr;
    addr.sa_family = AF_UNIX;
    strcpy(addr.sa_data, SOCK_NAME);

	char buf[BUF_SIZE];
    sprintf(buf, "pid %d", getpid());

	while (1)
	{
		if (sendto(fd, buf, strlen(buf), 0, &addr, strlen(addr.sa_data) + sizeof(addr.sa_family)) == -1)
		{
			printf("sendto() failed");
			close(fd);
			return EXIT_FAILURE;
		}

		printf("Client sent: %s\n", buf);

		sleep(3);
	}

    close(fd);
    return EXIT_SUCCESS;
}
