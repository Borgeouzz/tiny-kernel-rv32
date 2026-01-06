#include "kernel.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram[], __free_ram_end[];

/* ############## CONSOLE PRINT SECTION ############## */

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

/* ############## TRAP HANDLING SECTION ############## */

__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void) {
    __asm__ __volatile__(
        "csrw sscratch, sp\n"
        "addi sp, sp, -4 * 31\n"
        "sw ra,  4 * 0(sp)\n"
        "sw gp,  4 * 1(sp)\n"
        "sw tp,  4 * 2(sp)\n"
        "sw t0,  4 * 3(sp)\n"
        "sw t1,  4 * 4(sp)\n"
        "sw t2,  4 * 5(sp)\n"
        "sw t3,  4 * 6(sp)\n"
        "sw t4,  4 * 7(sp)\n"
        "sw t5,  4 * 8(sp)\n"
        "sw t6,  4 * 9(sp)\n"
        "sw a0,  4 * 10(sp)\n"
        "sw a1,  4 * 11(sp)\n"
        "sw a2,  4 * 12(sp)\n"
        "sw a3,  4 * 13(sp)\n"
        "sw a4,  4 * 14(sp)\n"
        "sw a5,  4 * 15(sp)\n"
        "sw a6,  4 * 16(sp)\n"
        "sw a7,  4 * 17(sp)\n"
        "sw s0,  4 * 18(sp)\n"
        "sw s1,  4 * 19(sp)\n"
        "sw s2,  4 * 20(sp)\n"
        "sw s3,  4 * 21(sp)\n"
        "sw s4,  4 * 22(sp)\n"
        "sw s5,  4 * 23(sp)\n"
        "sw s6,  4 * 24(sp)\n"
        "sw s7,  4 * 25(sp)\n"
        "sw s8,  4 * 26(sp)\n"
        "sw s9,  4 * 27(sp)\n"
        "sw s10, 4 * 28(sp)\n"
        "sw s11, 4 * 29(sp)\n"

        "csrr a0, sscratch\n"
        "sw a0, 4 * 30(sp)\n"

        "mv a0, sp\n"
        "call handle_trap\n"

        "lw ra,  4 * 0(sp)\n"
        "lw gp,  4 * 1(sp)\n"
        "lw tp,  4 * 2(sp)\n"
        "lw t0,  4 * 3(sp)\n"
        "lw t1,  4 * 4(sp)\n"
        "lw t2,  4 * 5(sp)\n"
        "lw t3,  4 * 6(sp)\n"
        "lw t4,  4 * 7(sp)\n"
        "lw t5,  4 * 8(sp)\n"
        "lw t6,  4 * 9(sp)\n"
        "lw a0,  4 * 10(sp)\n"
        "lw a1,  4 * 11(sp)\n"
        "lw a2,  4 * 12(sp)\n"
        "lw a3,  4 * 13(sp)\n"
        "lw a4,  4 * 14(sp)\n"
        "lw a5,  4 * 15(sp)\n"
        "lw a6,  4 * 16(sp)\n"
        "lw a7,  4 * 17(sp)\n"
        "lw s0,  4 * 18(sp)\n"
        "lw s1,  4 * 19(sp)\n"
        "lw s2,  4 * 20(sp)\n"
        "lw s3,  4 * 21(sp)\n"
        "lw s4,  4 * 22(sp)\n"
        "lw s5,  4 * 23(sp)\n"
        "lw s6,  4 * 24(sp)\n"
        "lw s7,  4 * 25(sp)\n"
        "lw s8,  4 * 26(sp)\n"
        "lw s9,  4 * 27(sp)\n"
        "lw s10, 4 * 28(sp)\n"
        "lw s11, 4 * 29(sp)\n"
        "lw sp,  4 * 30(sp)\n"
        "sret\n"
    );
}

/* hadle_trap handles trap from processor reading */
void handle_trap(struct trapframe *f) {
    uint32_t scause = READ_CSR(scause);
    uint32_t stval = READ_CSR(stval);
    uint32_t user_pc = READ_CSR(sepc);

    PANIC("\nunexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
}

/* ############## MEMORY ALLOCATION SECTION ############## */

/* alloc_pages is a simple memory allocation algorithm.
 * Instead of allocating in bytes like malloc, it allocates in a larger unit called "pages". 
 * of size PAGE_SIZE
 */
paddr_t alloc_pages(uint32_t n) {
    static paddr_t next_paddr = (paddr_t) __free_ram;
    paddr_t paddr = next_paddr;
    next_paddr += n * PAGE_SIZE;

    if (next_paddr > (paddr_t) __free_ram_end) {
        PANIC("out of memory");
    }
    memset((void *) paddr, 0, n * PAGE_SIZE);
    return paddr;
}

/* ############## PROCESSES SECTION ############## */

// list of processes and their execution context
struct process procs[MAX_PROCS];

struct process *create_process(uint32_t pc) {
    struct process *proc = NULL;
    int i;
    for (i=0; i<MAX_PROCS; i++) {
        if (procs[i].status == PROC_UNUSED) {
            proc = &procs[i];
            break;
        }
    }

    if (!proc)
        PANIC("no free process slots");

    // Stack callee-saved registers. These register values will be restored in
    // the first context switch in switch_context.
    uint32_t *sp = (uint32_t *) &proc->stack[sizeof(proc->stack)];
    // with loop restore registers s11-s0
    for (int j=0; j < 12; j++) {
        *--sp = 0;
    }
    *--sp = (uint32_t) pc; // ra

    // initialize process fields
    proc->pid = i+1;
    proc->status = PROC_RUNNABLE;
    proc->sp = (vaddr_t) sp;
    return proc;
}

/* switch_context switches execution context of processes.
 * It saves callee-saved registers and load other process context registers.
 */
__attribute__((naked))
void switch_context(uint32_t *prev_sp, uint32_t *next_sp) {
    __asm__ __volatile__(
        "addi sp, sp, -13 * 4\n" // move stack pointer up of 4 bytes for 13 registers
        "sw ra, 0 * 4(sp)\n"
        "sw s0, 0 * 4(sp)\n"
        "sw s1, 1 * 4(sp)\n"
        "sw s2, 2 * 4(sp)\n"
        "sw s3, 3 * 4(sp)\n"
        "sw s4, 4 * 4(sp)\n"
        "sw s5, 5 * 4(sp)\n"
        "sw s6, 6 * 4(sp)\n"
        "sw s7, 7 * 4(sp)\n"
        "sw s8, 8 * 4(sp)\n"
        "sw s9, 9 * 4(sp)\n"
        "sw s10, 10 * 4(sp)\n"
        "sw s11, 11 * 4(sp)\n"

        "sw sp, (a0)\n"
        "lw sp, (a1)\n"

        "lw ra, 0 * 4(sp)\n"
        "lw s0, 0 * 4(sp)\n"
        "lw s1, 1 * 4(sp)\n"
        "lw s2, 2 * 4(sp)\n"
        "lw s3, 3 * 4(sp)\n"
        "lw s4, 4 * 4(sp)\n"
        "lw s5, 5 * 4(sp)\n"
        "lw s6, 6 * 4(sp)\n"
        "lw s7, 7 * 4(sp)\n"
        "lw s8, 8 * 4(sp)\n"
        "lw s9, 9 * 4(sp)\n"
        "lw s10, 10 * 4(sp)\n"
        "lw s11, 11 * 4(sp)\n"
        "addi sp, sp, 13 * 4\n"
        "ret\n"
    );
}

void delay(void) {
    for (int i = 0; i < 30000000; i++)
        __asm__ __volatile__("nop"); // do nothing
}

struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void) {
    printf("\nstarting process: ");
    while (1) {
        put_char('A');
        switch_context(&proc_a->sp, &proc_b->sp);
        delay();
    }
}

void proc_b_entry(void) {
    printf("\nstarting process: ");
    while (1) {
        put_char('B');
        switch_context(&proc_b->sp, &proc_a->sp);
        delay();
    }
}

void kernel_main(void) {
    printf("\nbooting kernel...");
    printf("\nclearing bss section...");
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    WRITE_CSR(stvec, (uint32_t) kernel_entry);
    
    printf("\ntesting memory allocation");
    paddr_t paddr0 = alloc_pages(2);
    paddr_t paddr1 = alloc_pages(1);
    printf("\nalloc_pages test: paddr0=%x -- paddr1=%x", paddr0, paddr1);

    printf("\ntesting processes context switch");
    proc_a = create_process((uint32_t) proc_a_entry);
    proc_b = create_process((uint32_t) proc_b_entry);
    proc_a_entry();
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
