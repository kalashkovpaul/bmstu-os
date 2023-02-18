#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int flag = 0;

void check_status(int status);

void sig_handler(int signal);

int main()
{
	char *message[2][100] = {"FFF", "SSSSSSSSSSSSS"};
	pid_t child_pid[2];
    int fd[2];
    int status;
    int child;

	if (pipe(fd) == -1)
	{
		perror("Can't pipe");
		exit(1);
	}
	if (signal(SIGINT, sig_handler) == -1)
	{
		perror("Can't set signal handler");
		exit(1);
	}
    printf("Ctr + C to pipe\n");
	sleep(2);

	for (int i = 0; i < 2; i++)
	{
		if ((child_pid[i] = fork()) == -1)
		{
			perror("Can't fork");
			exit(1);
		}
		if (child_pid[i]==0)
		{
			if (flag)
            {
                close(fd[0]);
                write(fd[1], (*message)[i], strlen((*message)[i]));
                printf("Message from child (pid = %d) %s sent to parent\n", getpid(), (*message)[i]);
            }
            else
            {
                printf("No signal, thus no message will be recieved\n");
            }
			return 0;
		}

		else
        {
            printf("Parent pid: %d, child %d, group %d\n", getpid(), child_pid[i], getpgrp());
        }
	}
	for (int i = 0; i < 2; i++)
	{
		if (child_pid[i] = wait(&status) == -1)
		{
			perror("Can't wait");
			exit(1);
		}
		printf("child pid: %d, parent %d, status: %d, group %d\n", child_pid[i], getpid(), status, getpgrp());
		check_status(status);
	}
	close(fd[1]);
    read(fd[0], *message, sizeof(*message));
    printf("parent (pid: %d) recieved message: %s\n", getpid(), *message);
	return 0;
}

void sig_handler(int signal)
{
	flag = 1;
	printf("Catch signal %d\n", signal);
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