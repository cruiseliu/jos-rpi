#pragma once

#include <cstdarg>

/**
 * These functions are similar to standard libraries, so we omitted namespace
 * and descriptions.
 * stdin and stdout are set to the console, stderr not supported.
 */

struct FILE {
    void (*putc)(int ch);
    int (*getc)();
};

extern FILE * const stdin;
extern FILE * const stdout;

static inline int getchar()
{
    return stdin->getc();
}

// The standard one returns int, but we can't do anything if it fails.
static inline void putchar(int c)
{
    stdout->putc(c);
}

// Same to above.
static inline void puts(const char *s)
{
    for (; *s; ++s)
        putchar(*s);
    putchar('\n');
}

// We need this because puts always write trainling new line
static inline void fputs(const char *s, FILE *stream)
{
    for (; *s; ++s)
        stream->putc(*s);
}

int printf(const char *format, ...);

int vfprintf(FILE *stream, const char *format, va_list ap);

// Not a stdio function in fact
char * readline(const char *prompt);
