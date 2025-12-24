# tiny-kernel-rv32

A minimal operating system for RISC-V, built from scratch following the ["OS in 1,000 Lines"](https://operating-system-in-1000-lines.vercel.app/) guide.

## About

This project is a personal learning endeavor to understand how operating systems work at a fundamental level. It implements a basic kernel that can:

- Boot on RISC-V architecture
- Interact with OpenSBI for low-level hardware operations
- Output text to the console
- Provide a `printf` function for debugging
- Other implementations coming soon

## Requirements

- RISC-V compiler toolchain (e.g., `riscv32-unknown-elf-gcc`)
- QEMU with RISC-V support
- OpenSBI firmware

## Building and Running

```bash
./run.sh
```

This script compiles the kernel and runs it in QEMU. You should see "Hello World!" output along with example calculations.

## Project Structure

- `kernel.c` - Main kernel code
- `kernel.h` - Kernel headers and structures
- `common.c` - Shared utilities (printf, etc.)
- `common.h` - Shared headers
- `kernel.ld` - Linker script
- `run.sh` - Build and run script

## Learning Resources

Based on: [Operating System in 1,000 Lines](https://operating-system-in-1000-lines.vercel.app/en/05-hello-world)

## License

Personal project for learning purposes.

