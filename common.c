#include "common.h"

void put_char(char c);

/* print_debug allows to print a debug string.
 * It is used for example in printf to print an error on output
 * if the format is wrong.
 */
void print_debug(char *str) {
    while (*str) {
        put_char(*str);
        str++;
    }
}

void printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    while(*fmt) {
        if (*fmt == '%') {
            fmt++;  // skip % character
            switch (*fmt) {
                case '\0':
                    print_debug("printf wrong formatting. A format type is expected after \'%\'");
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
                    /* Unsigned a 32 bit -> 1 cifra esadecimale da 4 bit
                     * Example: 0x1234abcd
                     * value >> (i*4): 0x00000001
                     * 0x00000001 & 0xf: tiene solo i 4 bit meno significativi -> 0x0001
                     * "0123456789abcdef"[nibble]: nibble=1 -> viene stampato il carattere all'indice 1 
                     */
                    unsigned value = va_arg(ap, unsigned);
                    for (int i=7; i>=0; i--) {
                        unsigned nibble = (value >> (i*4)) & 0xf;
                        put_char("0123456789abcdef"[nibble]);
                    }
                    break;
                }

                default:
                    print_debug("invalid format. Format type must be in (\'d\', \'s\', \'x\')");
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
