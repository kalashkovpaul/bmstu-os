#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void print_status(int status);

int main()
{
	int child_pid[2];
	char exec_program[] = "./lab_05_01_06/a.out";
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
			if (execl(exec_program, NULL) == -1)
			{
				printf("Exec error");
				exit(1);
			}
			return 0;
		}
	}
	int status;
	pid_t childpid = 0;
	for (int i = 0; i < 2; i++)
	{
		childpid = waitpid(child_pid[i], &status, 0);
		printf("Status: %d, child_pid: %d\n",status,childpid);
		print_status(status);
		printf("Parent: pid: %d childpid: %d pgrp: %d\n", getpid(), child_pid[i], getpgrp());
		printf("\n");
	}
	return 0;
}

void print_status(int status)
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
