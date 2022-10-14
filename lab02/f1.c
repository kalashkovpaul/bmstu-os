#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define TIME 2

int main()
{
	pid_t child_pid[2];
	for (int i = 0; i < 2; i++)
	{
		if ((child_pid[i] = fork()) == -1)
		{
			perror("Can't fork :(\n");
			exit(1);
		}
		if (child_pid[i] == 0)
		{
			printf("child_%d: id %d ppid: %d pgrp: %d\n", i, getpid(), getppid(), getpgrp());
			sleep(TIME);
			printf("child_%d: id %d ppid: %d pgrp: %d\n", i, getpid(), getppid(), getpgrp());
			return 0;
		}
	   	else
		{
			printf("PARENT: id %d prgp: %d child_%d_pid: %d\n", getpid(), getpgrp(), i, child_pid[i]);
		}
	}
	return 0;
}
