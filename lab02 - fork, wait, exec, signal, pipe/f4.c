#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

void check_status(int status);

int main()
{
	pid_t child_pid[2];
	int fd[2];
	int childpid;
	char *message[2] = {"FFFFFFFFFFFFFFFFF", "SSS"};
	if (pipe(fd) == -1)
	{
		perror("Can't pipe :(\n");
		exit(1);
	}
	for (int i = 0; i < 2; i++)
	{
		if ((child_pid[i] = fork()) == -1)
		{
			perror("Can't fork :(\n");
			exit(1);
		}
		if (child_pid[i] == 0)
		{
			close(fd[0]);
			write(fd[1], message[i], (strlen(message[i]) + 1));
			printf("msg from child (pid = %d) %s sent to parent\n", getpid(), message[i]);
			return 0;
		}
		else
		{
			printf("parent pid: %d, child %d, group %d\n", getpid(), child_pid[i], getpgrp());
		}
	}
	for (int i = 0; i < 2; i++)
	{
		printf("PARENT: id %d prgp: %d child_%d: %d\n", getpid(), getpgrp(), i, child_pid[i]);
		int status;

		childpid = waitpid(child_pid[i], &status, 0);
		if (childpid == -1)
		{
			printf("waitpid failure\n");
		}
		else
		{
			printf("Process exit status: %d, child_pid: %d\n", status, childpid);
			check_status(status);
			close(fd[1]);
			read(fd[0], message[i], (strlen(message[i]) + 1));
			printf("%s\n", message[i]);
		}
	}
	return 0;
}

void check_status(int status)
{
	if (WIFEXITED(status))
		printf("Child exited all right. Exit code: %d\n", WEXITSTATUS(status));
	else
		printf("Child exited with error :(\n");

	if (WIFSIGNALED(status))
		printf("Signal: %d\n", WTERMSIG(status));
	else
		printf("Exited without any signal\n");

	if (WIFSTOPPED(status))
		printf("Exit signal: %d", WSTOPSIG(status));
	else
		printf("Process has not ended yet\n");
}
