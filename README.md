# IntrOS
  [![Build Status](https://travis-ci.org/stateos/IntrOS.svg)](https://travis-ci.org/stateos/IntrOS)
  [![Github Release](https://img.shields.io/github/release/stateos/IntrOS.svg)](https://github.com/stateos/IntrOS/releases)
  [![Software License](https://img.shields.io/github/license/stateos/IntrOS.svg)](https://opensource.org/licenses/MIT)
  [![Software Download](https://img.shields.io/sourceforge/dt/intros.stateos.p.svg)](https://sourceforge.net/projects/intros.stateos.p/files/latest/download)

Free, simple and tiny cooperative operating system (OS) designed for deeply embedded applications.
Target: ARM Cortex-M, STM8, AVR8 families.
It was inspired by the concept of a state machine.
Procedure executed by the task (task state) doesn't have to be noreturn-type.
It will be executed into an infinite loop.
There's a dedicated function for immediate change the task state.

### Features

- kernel can operate in cooperative (non-preemptive) mode only
- kernel can operate with 16, 32 or 64-bit timer counter
- kernel can operate in tick-less mode
- spin locks
- signals (auto clearing, protected)
- events
- flags (any, all)
- barriers
- semaphores
- mutexes
- condition variables
- memory pools
- stream buffers
- message buffers
- mailbox queues
- job queues
- event queues
- timers (one-shot, periodic)
- c++ wrapper
- all documentation is contained within the source files
- examples and templates are in separate repositories on [GitHub](https://github.com/stateos)
- archival releases on [sourceforge](https://sourceforge.net/projects/intros.stateos.p)

### Targets

ARM CM0(+), CM3, CM4(F), CM7

### License

This project is licensed under the terms of [MIT License (MIT)](https://opensource.org/licenses/MIT).
