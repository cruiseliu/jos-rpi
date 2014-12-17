#include "string.h"

size_t strlen(const char *s)
{
    size_t ret = 0;
    for (; s[ret]; ++ret) { }
    return ret;
}

size_t strnlen(const char *s, size_t maxlen)
{
    size_t ret = 0;
    for (; ret < maxlen && s[ret]; ++ret) { }
    return ret;
}

int strcmp(const char *s1, const char *s2)
{
    for (; *s1 && *s1 == *s2; ++s1, ++s2) { }
    return *s2 - *s1;
}

const char * strchr(const char *s, int c)
{
    for (; *s && *s != c; ++s) { }
    return *s == c ? s : nullptr;
}
