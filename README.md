# IntrOS
  [![](https://img.shields.io/github/release/stateos/IntrOS.svg?style=flat-square&logo)](https://github.com/stateos/IntrOS/releases)
  [![](https://img.shields.io/github/license/stateos/IntrOS.svg?style=flat-square&logo)](https://opensource.org/licenses/MIT)
  [![](https://github.com/stateos/IntrOS/actions/workflows/test.yml/badge.svg)](https://github.com/stateos/IntrOS/actions/workflows/test.yml)

Free, simple and tiny cooperative operating system (OS) designed for deeply embedded applications.
Target: ARM Cortex-M, STM8, AVR8.
It was inspired by the concept of a state machine.
Procedure executed by the task (task state) doesn't have to be noreturn-type.
It will be executed into an infinite loop.
There's a dedicated function for immediate change the task state.

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
- flags (any, all)
- barriers
- semaphores
- mutexes
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
- all documentation is contained within the source files
- examples and templates are in separate repositories on [GitHub](https://github.com/stateos)
- archival releases on [sourceforge](https://sourceforge.net/projects/intros.stateos.p)

### Targets

ARM CM0(+), CM3, CM4(F), CM7

### License

This project is licensed under the terms of [MIT License (MIT)](https://opensource.org/licenses/MIT).
