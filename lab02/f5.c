#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

void print_status(int status);

void sig_handler(int signal);

_Bool flag = false;

int main() {
	signal(SIGINT, sig_handler);
	int child_pid[2];
	int fd[2];
	char *message[2] = {"FFFFFFFF", "SSS"};
	int msg_len = 8;
	if (pipe(fd) == -1)
	{
		perror("Can't pipe :(\n");
		exit(1);
	}
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
			write(fd[1], message[i], (strlen(message[i]) + 1) * sizeof(char));
			printf("Message from child pid = %d %s sent to parent\n", getpid(), message[i]);
			return 0;
		}
		else
		{
			printf("parent pid: %d, child %d, group %d\n", getpid(), child_pid[i], getpgrp());
		}
	}

	if (child_pid[0] && child_pid[1])
	{
		pid_t child_pid;
		int status;

		close(fd[1]);

		read(fd[0], message[0], msg_len);
		read(fd[0], message[1], msg_len);

		printf("PARENT\n");
		printf("Ctrl + C = child_1, else child_2\nWaiting 3 sec...\n\n");
		sleep(3);
		if (flag)
		{
			printf("CHILD_1\n");
			printf("%s\n", message[0]);
		}
		else
		{
			printf("CHILD_2\n");
			printf("%s\n", message[1]);
		}

		for (int i = 0; i < 2; i++)
		{
			child_pid = wait(&status);
			printf("Status: %d, child_pid: %d\n",status,child_pid);
			print_status(status);
			printf("\n");
		}
	}

	printf("PARENT: id %d prgp: %d child_1_pid: %d, child_2_pid: %d\n", getpid(), getpgrp(), child_pid[0], child_pid[1]);
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
void sig_handler(int signal)
{
	flag = true;
	printf("Caught signal: %d\n", signal);
}
