#include <stdio.h>
#define OK 0
#define ERR_NO_DATA -1

int process(FILE *f, int *result)
{
    int number, previous;
    int result_length = -1, current_length = 0, increasing;
    if (fscanf(f, "%d", &previous) == 1)
    {
        current_length++;
        if (fscanf(f, "%d", &number) == 1)
        {
            if (number != previous)
                current_length++;
            increasing = number > previous;
            previous = number;
            int is_third = 0;
            while (fscanf(f, "%d", &number) == 1)
            {
                is_third = 1;
                if ((previous != number) && ((number > previous && increasing) || (number < previous && !increasing)))
                    current_length++;
                else
                {
                    increasing = !increasing;
                    if (current_length > result_length)
                        result_length = current_length;
                    if (number != previous)
                        current_length = 2;
                    else
                        current_length = 1;
                }
                previous = number;
            }
            if (!is_third)
                return ERR_NO_DATA;
        }
        else
            return ERR_NO_DATA;
        if (current_length > result_length)
            result_length = current_length;
    }
    else
        return ERR_NO_DATA;
    if (result_length == 1)
        result_length = 0;
    *result = result_length;
    return OK;
}