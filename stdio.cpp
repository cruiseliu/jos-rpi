#include "stdio.h"
#include "string.h"
#include "console.h"

// Because we only have kernel, just crash if write to stdin or read from stdout
static FILE file_stdin  = { nullptr, Console::getc };
static FILE file_stdout = { Console::putc, nullptr };

FILE * const stdin  = &file_stdin;
FILE * const stdout = &file_stdout;

int printf(const char *format, ...)
{
    va_list arg;
    int ret;

    va_start(arg, format);
    ret = vfprintf(stdout, format, arg);
    va_end(arg);

    return ret;
}

const int buflen = 1024;
static char buf[buflen];

char * readline(const char *prompt)
{
    if (prompt)
        fputs(prompt, stdout);

    int len = 0;
    while (true) {
        char ch = getchar();
        // The UART will send us a \x7f (delete) when we press backspace
        if (ch == '\x7f') ch = '\b';
        // Always echo
        putchar(ch);

        if (ch == '\n') {
            buf[len] = '\0';
            return buf;
        } else if (ch == '\b') {
            --len;
        } else {
            buf[len++] = ch;
        }
    }
}

/* vfprintf part, mostly copied from JOS */

const char digits[] = "0123456789abcdef";

// Print dec number, using software division.
static int printnum_d(FILE *s, unsigned long long num, int width, int padc)
{
    int ret = 0;
    if (num >= 10) {
        ret += printnum_d(s, num / 10, width - 1, padc);
    } else {
        ret += width;
        while (--width > 0)
            s->putc(padc);
    }
    s->putc(digits[num % 10]);
    return ret + 1;
}

// Print oct or hex number, using shift.
static int printnum_ox(FILE *s, unsigned long long num, unsigned log_base, int width, int padc)
{
    int ret = 0;

    // make sure shift operation generates a unsigned long long
    unsigned long long tmp = 1;
    tmp <<= log_base;

    if (num >= tmp) {
        ret += printnum_ox(s, num >> log_base, log_base, width - 1, padc);
    } else {
        ret += width;
        while (--width > 0)
            s->putc(padc);
    }
    s->putc(digits[num & (tmp - 1)]);
    return ret + 1;
}

static unsigned long long getuint(va_list *ap, int lflag)
{
    if (lflag >= 2)
        return va_arg(*ap, unsigned long long);
    else if (lflag)
        return va_arg(*ap, unsigned long);
    else
        return va_arg(*ap, unsigned int);
}

static long long getint(va_list *ap, int lflag)
{
    if (lflag >= 2)
        return va_arg(*ap, long long);
    else if (lflag)
        return va_arg(*ap, long);
    else
        return va_arg(*ap, int);
}

int vfprintf(FILE *s, const char *fmt, va_list ap)
{
    int ret = 0;
    char ch, padc;
    int width, precision, lflag, altflag, log_base;
    unsigned long long num;
    const char *p;

    while (true) {
        while ((ch = *fmt++) != '%') {
            if (ch == '\0')
                return ret;
            s->putc(ch);
            ++ret;
        }

        padc = ' ';
        width = -1;
        precision = -1;
        lflag = 0;
        altflag = 0;
    reswitch:
        switch (ch = *fmt++) {
        case '-':
            padc = '-';
            goto reswitch;

        case '0':
            padc = '0';
            goto reswitch;
            
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            for (precision = 0; ; ++fmt) {
                precision = precision * 10 + ch - '0';
                ch = *fmt;
                if (ch < '0' || ch > '9')
                    break;
            }
            goto process_precision;

        case '*':
            precision = va_arg(ap, int);
            goto process_precision;

        case '.':
            if (width < 0)
                width = 0;
            goto reswitch;

        case '#':
            altflag = 1;
            goto reswitch;

    process_precision:
            if (width < 0) {
                width = precision;
                precision = -1;
            }
            goto reswitch;

        case 'l':
            lflag++;
            goto reswitch;

        case 'c':
            s->putc(va_arg(ap, int));
            ++ret;
            break;

        //case 'e':

        case 's':
            if ((p = va_arg(ap, char *)) == nullptr)
                p = "(null)";
            if (width > 0 && padc != '-') {
                width -= strnlen(p, precision);
                ret += width;
                for (; width > 0; --width)
                    s->putc(padc);
            }
            for (; (ch = *p++) != '\0' && (precision < 0 || --precision >= 0); --width) {
                if (altflag && (ch < ' ' || ch > '~'))
                    s->putc('?');
                else
                    s->putc(ch);
                ++ret;
            }
            ret += width;
            for (; width > 0; --width)
                s->putc(' ');
            break;

        case 'd':
            num = getint(&ap, lflag);
            if ((long long) num < 0) {
                s->putc('-');
                ++ret;
                num = -(long long) num;
            }
            goto dec_number;

        case 'u':
            num = getuint(&ap, lflag);

        dec_number:
            printnum_d(s, num, width, padc);
            break;

        case 'o':
            num = getuint(&ap, lflag);
            log_base = 3; // log2(8)
            goto number;

        case 'p':
            s->putc('0');
            s->putc('x');
            ret += 2;
            num = (unsigned long long) (uintptr_t) va_arg(ap, void *);
            log_base = 4; // log2(16)
            goto number;

        case 'x':
            num = getuint(&ap, lflag);
            log_base = 4; // log2(16)

        number:
            printnum_ox(s, num, log_base, width, padc);
            break;

        case '%':
            s->putc(ch);
            ++ret;
            break;

        default:
            s->putc('%');
            for (fmt--; fmt[-1] != '%'; fmt--) { }
            break;
        }
    }
}
