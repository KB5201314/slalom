#include <util.h>
#include <stddef.h>
#include <ctype.h>

#include <tee_internal_api.h>

int strncasecmp(const char *s1, const char *s2, size_t n) {
    int d = 0;
    for (; n != 0; n--) {
        const int c1 = tolower(*s1++);
        const int c2 = tolower(*s2++);
        if (((d = c1 - c2) != 0) || (c2 == '\0'))
            break;
    }
    return d;
}

int strcasecmp(const char *s1, const char *s2) {
    int d = 0;
    for (;;) {
        const int c1 = tolower(*s1++);
        const int c2 = tolower(*s2++);
        if (((d = c1 - c2) != 0) || (c2 == '\0'))
            break;
    }
    return d;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0)
        return 0;

    while (n-- != 0 && *s1 == *s2) {
        if (n == 0 || *s1 == '\0')
            break;
        s1++;
        s2++;
    }

    return (*(unsigned char *)s1) - (*(unsigned char *)s2);
}

/*
 * Convert a string to a long integer.
 */
static long _strtol_l(const char *__restrict nptr, char **__restrict endptr,
                      int base) {
    register const unsigned char *s = (const unsigned char *)nptr;
    register unsigned long acc;
    register int c;
    register unsigned long cutoff;
    register int neg = 0, any, cutlim;

    if (base < 0 || base == 1 || base > 36) {
        // errno = EINVAL;
        return 0;
    }

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
    cutlim = cutoff % (unsigned long)base;
    cutoff /= (unsigned long)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) {
        acc = neg ? LONG_MIN : LONG_MAX;
        // rptr->_errno = ERANGE;
    } else if (neg)
        acc = -acc;
    if (endptr != 0)
        *endptr = (char *)(any ? (char *)s - 1 : nptr);
    return (acc);
}

long _strtol_r(const char *__restrict nptr, char **__restrict endptr,
               int base) {
    return _strtol_l(nptr, endptr, base);
}

long strtol(const char *__restrict s, char **__restrict ptr, int base) {
    return _strtol_l(s, ptr, base);
}

int atoi(const char *s) { return (int)strtol(s, NULL, 10); }

// Copy from David M. Gay's implements of strtod()
// https://www.ampl.com/netlib/fp/dtoa.c
#define __MATH_H__
#define IEEE_8087
#define Long int
#define Bad_float_h
#define NO_ERRNO
#include "dtoa.c"

double atof(const char *s) { return strtod(s, NULL); }

void __assert_fail(const char *assertion, const char *file, unsigned int line,
                   const char *function) {
    DMSG("Assert failed: %s at %s:%s in function %s\n", assertion, file, line,
         function);
    abort();
}
