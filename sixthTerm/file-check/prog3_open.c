#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char *alphabet = "abcdefghijklmnopqrstuvwxyz";
    int fd1 = open("out.txt", O_WRONLY|O_CREAT);
    int fd2 = open("out.txt",O_WRONLY);
    printf("pid %d fd1 %d fd2 %d Continue?", getpid(), fd1, fd2);
    char c;
    scanf("%c", &c);
    printf("%c", c);
    for (char i = 0; i < 26; i++)
    {
        i % 2 ? write(fd1, alphabet + i,1) : write(fd2,alphabet + i,1);
        printf("%c", *(alphabet + i));
    }
    scanf("%c", &c);
    printf("%c", c);
    close(fd1);
    close(fd2);
    scanf("%c", &c);
    printf("%c", c);
    return 0;
}