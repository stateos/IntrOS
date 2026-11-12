# IntrOS
  [![](https://img.shields.io/github/release/stateos/IntrOS.svg?style=flat-square&logo)](https://github.com/stateos/IntrOS/releases)
  [![](https://img.shields.io/github/license/stateos/IntrOS.svg?style=flat-square&logo)](https://opensource.org/licenses/MIT)
  [![](https://github.com/stateos/IntrOS/actions/workflows/test.yml/badge.svg)](https://github.com/stateos/IntrOS/actions/workflows/test.yml)
  [![](https://github.com/stateos/IntrOS/actions/workflows/docs.yml/badge.svg)](https://stateos.github.io/IntrOS/)

Free, simple and tiny cooperative operating system (OS) designed for deeply embedded applications.
Target: ARM Cortex-M, STM8, AVR8, x86_64.
It was inspired by the concept of a state machine.
Procedure executed by the task (task state) doesn't have to be noreturn-type.
It will be executed into an infinite loop.
There's a dedicated function for immediate change the task state.

### Documentation

The API is documented in the header files, the documentation generated with doxygen is available on [GitHub Pages](https://stateos.github.io/IntrOS/).
How to port the system to another processor, compiler or device is described in [Porting IntrOS](https://stateos.github.io/IntrOS/porting.html).

### Getting started

Clone the repository with the submodule `common` (kernel, ports, startup code, CMSIS, build scripts):

```
git clone --recursive https://github.com/stateos/IntrOS.git
```

- `include/osconfig.h`: configuration of the kernel
- `src/main.cpp`: the application (`src/main.c` in C)
- `examples`: complete programs that show the use of every object, in C and C++

The project is built for the STM32F4-Discovery board:

- make: `make -f makefile.gnucc` (or `makefile.clang`, `makefile.armcc`, `makefile.iarcc`, `makefile.csmcc`)
- CMake: `cmake -S . -B build -G Ninja` and `cmake --build build`

Another board is selected in the makefile (`include $(COMMON)/intros/make/<board>/makefile.<compiler>`) or in `CMakeLists.txt` (`CMAKE_TOOLCHAIN_FILE`);
the supported boards are in `common/intros/make` and `common/intros/cmake`.

### Features

- kernel can operate in cooperative (non-preemptive) mode only
- kernel can operate with 16, 32 or 64-bit timer counter
- kernel can operate in tick-less mode
- implemented basic protection using MPU (use nullptr, stack overflow)
- implemented functions for asynchronous communication with unmasked interrupt handlers
- spin locks
- once flags
- events
- signals with protection mask
- flags (any, all, new)
- barriers
- semaphores (binary, limited, counting)
- mutexes (error check)
- condition variables
- read/write locks
- memory pools
- raw buffers
- message queues
- mailbox queues
- event queues
- job queues
- timers (one-shot, periodic)
- hierarchical state machine
- c++ wrapper
- [API documentation](https://stateos.github.io/IntrOS/) generated with doxygen from the header files
- examples and templates are in separate repositories on [GitHub](https://github.com/stateos)
- archival releases on [sourceforge](https://sourceforge.net/projects/intros.stateos.p)

### Targets

- ARM Cortex-M0(+), M3, M4(F), M7: compilers armcc, clang, csmcc, gnucc, iarcc
- STM8: compilers csmc, iarc, sdcc
- AVR8: compiler gcc
- x86_64 (PC: Linux, Windows): compiler gcc, for testing purposes

### License

This project is licensed under the terms of the [MIT License (MIT)](https://opensource.org/licenses/MIT).
