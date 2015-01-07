#include "stdlib.h"

int atoi(const char *str, int base)
{
    int ret = 0;
    for (; *str; ++str) {
        if (*str >= 'a')
            ret = ret * base + *str - 'a' + 10;
        else if (*str >= 'A')
            ret = ret * base + *str - 'A' + 10;
        else
            ret = ret * base + *str - '0';
    }
    return ret;
}
