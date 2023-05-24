#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char c;    
    int fd1 = open("alphabet.txt",O_RDONLY);
    int fd2 = open("alphabet.txt",O_RDONLY);

    int flag1 = 1, flag2 = 2;
    while(flag1 == 1 || flag2 == 1)
    {
        char c;
        flag1 = read(fd1,&c,1);
        if (flag1 == 1) 
        {
            write(1,&c,1);
        }
        flag2 = read(fd2,&c,1);
        if (flag2 == 1) 
        { 
            write(1,&c,1);
        }
    }
    printf("\n");
    return 0;
}