#include "stdio.h"

/* Ugly, rewrite me */

int printf(const char *format, ...)
{
    va_list arg;
    int ret;

    va_start(arg, format);
    ret = vfprintf(stdout, format, arg);
    va_end(arg);

    return ret;
}

static int printnum(unsigned long long num, unsigned base, int width, int padc)
{
    int ret = 0;
    if (num >= base) {
        ret += printnum(num / base, base, width - 1, padc);
    } else {
        ret += width;
        while (--width > 0)
            putchar(padc);
    }
    putchar("0123456789abcdef"[num % base]);
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

// TODO: move it to string.c
static int strnlen(const char *s, int size)
{
    int n;
    for (n = 0; size > 0 && *s != '\0'; ++s, --size)
        ++n;
    return n;
}

// FIXME: stream is ignored
int vfprintf(__attribute__ ((unused))FILE *stream, const char *fmt, va_list ap)
{
    int ret = 0;
    char ch, padc;
    int width, precision, lflag, altflag, base;
    unsigned long long num;
    const char *p;

    while (true) {
        while ((ch = *fmt++) != '%') {
            if (ch == '\0')
                return ret;
            putchar(ch);
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
            putchar(va_arg(ap, int));
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
                    putchar(padc);
            }
            for (; (ch = *p++) != '\0' && (precision < 0 || --precision >= 0); --width) {
                if (altflag && (ch < ' ' || ch > '~'))
                    putchar('?');
                else
                    putchar(ch);
                ++ret;
            }
            ret += width;
            for (; width > 0; --width)
                putchar(' ');
            break;

        case 'd':
            num = getint(&ap, lflag);
            if ((long long) num < 0) {
                putchar('-');
                ++ret;
                num = -(long long) num;
            }
            base = 10;
            goto number;

        case 'u':
            num = getuint(&ap, lflag);
            base = 10;
            goto number;

        case 'o':
            num = getuint(&ap, lflag);
            base = 8;
            goto number;

        case 'p':
            putchar('0');
            putchar('x');
            ret += 2;
            num = (unsigned long long) (uintptr_t) va_arg(ap, void *);
            base = 16;
            goto number;

        case 'x':
            num = getuint(&ap, lflag);
            base = 16;

        number:
            printnum(num, base, width, padc);
            break;

        case '%':
            putchar(ch);
            ++ret;
            break;

        default:
            putchar('%');
            for (fmt--; fmt[-1] != '%'; fmt--) { }
            break;
        }
    }
}

// copied somewhere
//extern "C"
//uint64_t __aeabi_uidivmod(uint32_t value, uint32_t divisor) {
//    uint64_t answer = 0;
//
//    for (uint32_t i = 0; i < 32; i++) {
//        if ((divisor << (31 - i)) >> (31 - i) == divisor) {
//            if (value >= divisor << (31 - i)) {
//                value -= divisor << (31 - i);
//                answer |= 1 << (31 - i);
//                if (value == 0) break;
//            } 
//        }
//    }
//
//    answer |= (uint64_t)value << 32;
//    return answer;
//}
//
//extern "C"
//uint32_t __aeabi_uidiv(uint32_t value, uint32_t divisor) {
//    return __aeabi_uidivmod(value, divisor);
//}

const int buflen = 1024;
static char buf[buflen];

char * readline(const char *prompt)
{
    if (prompt)
        printf("%s", prompt);

    int len = 0;
    while (true) {
        char ch = getchar();
        if (ch == '\x7f') ch = '\b';
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
