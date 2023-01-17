#include <stdio.h>
#include "process.h"
#define OK 0

int main(void)
{
    int result = 0, check;
    FILE *f = fopen("./lab_05_01_06/func_tests/pos_01_in.txt", "r");
    int choice = -1;
    printf("Choose sort: ");
    scanf("%d", &choice);
    check = process(f, &result);
    if (check == OK)
        printf("Result: %d\n", result);
    fclose(f);
    return check;
}
