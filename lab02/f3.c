#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void check_status(int status);

int main()
{
	int child_pid[2];
	pid_t childpid;
	int status;
	char *exec_program[] = {"./lab_05_01_06/a1.out", "./lab_05_01_06/a.out"};
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
			if (execl(exec_program[i], NULL) == -1)
			{
				printf("Exec error");
				exit(1);
			}
			return 0;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		childpid = waitpid(child_pid[i], &status, 0);
		if (childpid == -1)
		{
			printf("waitpid failure\n");
		}
		else
		{
			printf("Process exit status: %d, child_pid: %d\n", status, childpid);
			check_status(status);
			printf("Parent: pid: %d pgrp: %d childpid: %d\n", getpid(), getpgrp(), child_pid[i]);
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
