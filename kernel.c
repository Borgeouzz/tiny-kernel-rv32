#include "kernel.h"
#include "common.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

/* sbi_call copies arguments into register following SBI standard registers use.
 * It calls SBI specific function (using fid and eid) using ecall.
 */
struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid) {
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;
    
    __asm__ __volatile__("ecall"
                        : "=r"(a0), "=r"(a1) 
                        : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                        : "memory");
    return (struct sbiret){.error = a0, .value = a1};
}

void put_char(char c) {
    // write functionID and extensionID on a6 and a7 registers.
    // OpenSBI extension ID for console putchar is 0x01.
    sbi_call(c,0,0,0,0,0,0,1);
}

void kernel_main(void) {
    printf("\nHello, World!");
    printf("\n%s: integer -> %d --- hex: %x", "Testing printf", 10, 0x1234abcd);
    printf("\nTesting format error %");
    printf("\nTesting invalid format type: %a", 1);

    for (;;) {
        __asm__ __volatile__("wfi");
    }
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // set the stack pointer at the stack end address
        "j kernel_main\n"       // jump to kernel main function
        :
        : [stack_top] "r" (__stack_top) // pass the stack top address as %[stack_top]
    );
}
