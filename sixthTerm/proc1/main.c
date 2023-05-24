#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

int scan_file(char* path) {
    FILE *f =fopen(path, "r");
    char buf[1000] = {0};
    int len = 0;

    while ((len = fread(buf, 1, 1000, f)) > 0)
    {
        for (int i = 0; i < len; i++)
            if (buf[i] == 0)
                buf[i] = 10;
        buf[len - 1] = 0;
        printf("%s", buf);
    }
    printf("\n");
    fclose(f);
}

int scan_stat(char* path) {
    char buf[1000] = {0};
    int len = 0;

    FILE *f = fopen(path, "r");
    fread(buf, 1, 1000, f);
    char *pch = strtok(buf, " ");
    // printf("%s", buf);
    
    printf("STAT: \n");
    while(pch != NULL)
    {
        printf("%s\n", pch);
        pch = strtok(NULL, " ");
    }
    fclose(f);
}

void read_dir(char *path)
{
    printf("\nfd %s:\n", path);
    struct dirent *dirp;
    DIR *dp;
    char str[1000] = {0};
    char local_path[1000] = {0};
    dp = opendir(path);
    while ((dirp = readdir(dp)) != NULL)
    {
        if ((strcmp(dirp->d_name, ".") != 0) &&
            (strcmp(dirp->d_name, "..") != 0))
        {
            sprintf(local_path, "%s%s", path, dirp->d_name);
            readlink(local_path, str, 1000);
            printf("%s -> %s\n", dirp->d_name, str);
        }
    }
    closedir(dp);
}

void scan_map(char* path)
{
    char *line;
    int start_addr, end_addr, page_size = 4096;
    size_t line_size;
    ssize_t line_length;

    char buf[1000] = {'\0'};
    FILE *file = fopen(path, "r");
    int lengthOfRead;
    int total = 0;
    int current = 0;
    do
    {
        line_length = getline(&line, &line_size, file);
        if (!feof(file) && line_length == -1)
        {
            perror("getline():");
            fclose(file);
            free(line);
            exit(1);
        }
        sscanf(line, "%x-%x", &start_addr, &end_addr);
        current = (end_addr - start_addr);
        printf("%d\t%s", (end_addr - start_addr) / page_size, line);
        total += current;
    } while (!feof(file));
    printf("Total: %d\n", total);
    fclose(file);
} 

int scan_dir(char* path)
{
    DIR *d = opendir(path);
    struct dirent *dir;
    if (d)
    {
        while((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    printf("\n");
}

int main(int argc, char**argv)
{
    char env_path[30] = {0};
    char stat_path[30] = {0};
    char cmdline_path[30] = {0};
    char fd_path[30] = {0};
    char maps_path[30] = {0};
    char task_path[30] = {0};
    char pagemap_path[30] = {0};
    snprintf(&env_path, 30, "/proc/%s/environ", argv[1]);
    snprintf(&stat_path, 30, "/proc/%s/stat", argv[1]);
    snprintf(&cmdline_path, 30, "/proc/%s/cmdline", argv[1]);
    snprintf(&fd_path, 30, "/proc/%s/fd/", argv[1]);
    snprintf(&maps_path, 30, "/proc/%s/maps", argv[1]);
    snprintf(&task_path, 30, "/proc/%s/task/", argv[1]);
    snprintf(&pagemap_path, 30, "/proc/%s/pagemap", argv[1]);


    printf("ENV:\n");
    scan_file(env_path);
    printf("STAT:\n");
    scan_stat(stat_path);
    printf("CMDLINE\n");
    scan_file(cmdline_path);
    read_dir(fd_path);

    printf("MAPS\n");
    scan_map(maps_path);

    printf("TASK\n");
    scan_dir(task_path);

    printf("PAGEMAP");
    scan_dir(pagemap_path);

    return 0;
}

// + task + pagemap