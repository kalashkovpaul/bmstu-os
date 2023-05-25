#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *thread_routine(void *arg)
{
    int fd = *((int *)arg);

    int flag = 1;
    char c;

    while (flag == 1)
    {
        flag = read(fd, &c, 1);
        if (flag == 1)
            write(1, &c, 1);
    }
}

int main()
{
    int fd1 = open("alphabet.txt", O_RDONLY);
    int fd2 = open("alphabet.txt", O_RDONLY);

    pthread_t thr_worker;

    pthread_create(&thr_worker, NULL, thread_routine, &fd1);

    int flag = 1;
    char c;

    while (flag == 1)
    {
        flag = read(fd2, &c, 1);
        if (flag == 1)
            write(1, &c, 1);
    }

    pthread_join(thr_worker, NULL);
    printf("\n");
    return 0;
}