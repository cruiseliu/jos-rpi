#pragma once

#include <cstdarg>
#include "console.h" // getchar, putchar, puts

struct FILE { };

FILE * const stdin = nullptr;
FILE * const stdout = nullptr;

int printf(const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list ap);

char * readline(const char *prompt);
