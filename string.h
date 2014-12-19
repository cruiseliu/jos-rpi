#pragma once

#include <cstddef>

/**
 * @file
 * @brief Standard functions, that's all.
 */

size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);
int strcmp(const char *s1, const char *s2);
const char * strchr(const char *s, int c);
const char * strchrnul(const char *s, int c);
