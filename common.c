#include "common.h"

void put_char(char c);

/*########## MEMORY FUNCTIONS ##########*/

/* memset copies the character c to the first n characters of the buffer pointed to. 
 * It returns a pointer to the first character of the buffer.
 */
void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c;    
    return buf;
}

/* memcpy copies n bytes from one location (src) to another (dst).
 * It returns a pointer pointing at the start of destination memory buffer.
 */
void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *) dst;
    const uint8_t *s = (const uint8_t *) src;
    while (n--)
        *d++ = *s++;
    return dst;
}

/*########## STRINGS MANIPULATION FUNCTIONS ##########*/

/* strcpy copies the string pointed by source (including the null character) to the destination. 
 * It returns a pointer to destination string.
 */
char *strcpy(char *dst, const char *src) {
    char *d = dst;
    while (*src) 
        *d++ = *src++;
    *d = '\0';
    return dst;
}

/* strcmp compares two strings character by character. 
 * If the strings are equal, the function returns 0
 */
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2)
            break;
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/* printf allows to output formatted text to standard output stream.
 * It supports the following format types:
 * 'd': integer
 * 's': string
 * 'x': hexadecimal
 */
void printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    while(*fmt) {
        if (*fmt == '%') {
            fmt++;  // skip % character
            switch (*fmt) {
                case '\0':
                    printf("invalid printf format string");
                    goto end;

                case PRINTF_TYPE_INT: {// 'd'
                    int value = va_arg(ap, int);
                    unsigned magnitude = value;
                    if (value < 0) {
                        put_char('-');
                        magnitude = -magnitude;
                    }
                    
                    /* Find number divisor. Number 1234 has 4 characters: '1','2','3','4'.
                     * The implementation finds the biggest divisor (1000 in this example).
                     * It divides the number by the divisor, print the obtained character,
                     * calculates the rest of division (234) and repeat this loop till divisor
                     * is < 0.
                     */
                    unsigned divisor = 1;
                    while (magnitude/divisor > 9)
                        divisor *= 10;
                    
                    while (divisor > 0) {
                        put_char('0'+magnitude/divisor);
                        magnitude %= divisor;
                        divisor /= 10;
                    }
                    break;
                }

                case PRINTF_TYPE_STR: {// 's'
                    const char *str = va_arg(ap, const char *);
                    while(*str) {
                        put_char(*str);
                        str++;
                    }
                    break;
                }

                case PRINTF_TYPE_HEX: { // 'x'
                    /* Unsigned a 32 bit -> one 4 bit hexadecimal digit
                     * Example: 0x1234abcd
                     * value >> (i*4): 0x00000001
                     * 0x00000001 & 0xf: only keeps 4 less significative bits -> 0x0001
                     * "0123456789abcdef"[nibble]: nibble=1 -> prints char at index 1 
                     */
                    unsigned value = va_arg(ap, unsigned);
                    for (int i=7; i>=0; i--) {
                        unsigned nibble = (value >> (i*4)) & 0xf;
                        put_char("0123456789abcdef"[nibble]);
                    }
                    break;
                }

                default:
                    printf("format type must be in (\'d\', \'s\', \'x\')");
                    goto end;
            }
        } else {
            put_char(*fmt);
        }
        fmt++;
    }

end:
    va_end(ap);
}
