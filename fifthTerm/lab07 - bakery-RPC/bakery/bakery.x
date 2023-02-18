/*
 * filename: bakery.x
     * function: Define constants, non-standard data types and the calling process in remote calls
 */

const N = 5;

struct BAKERY
{
    int number;
    int id;
    char letter;
};

program BAKERY_PROG
{
    version BAKERY_VER
    {
        struct BAKERY BAKERY_PROC(struct BAKERY) = 1;
    } = 1; /* Version number = 1 */
} = 0x20000001; /* RPC program number */