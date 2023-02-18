#include <stdio.h>
#include "file_find_max.h"
#include "file_find_min.h"
#include "file_how_many_bigger.h"
#define OK 0
#define ERR_IO -1

int main(int argc, char **argv)
{
    if (argc != 2)
        return ERR_IO;
    double max, min, avg;
    int result = 0;
    int check = (file_find_max(argv[1], &max) || file_find_min(argv[1], &min));
    if (check != OK)
        return check;
    avg = (max + min) / 2;
    file_how_many_bigger(argv[1], avg, &result);
    printf("%d\n", result);
    return 0;
}
