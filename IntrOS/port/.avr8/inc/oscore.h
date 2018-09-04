/******************************************************************************

    @file    IntrOS: oscore.h
    @author  Rajmund Szymanski
    @date    16.07.2018
    @brief   IntrOS port file for AVR8 uC.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __INTROSCORE_H
#define __INTROSCORE_H

#include "osbase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_STACK_SIZE
#define OS_STACK_SIZE        64 /* default task stack size in bytes           */
#endif

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#ifndef OS_FUNCTIONAL

#define OS_FUNCTIONAL         0 /* c++ functional library header not included */

#elif   OS_FUNCTIONAL

#error  c++ functional library not allowed for this compiler.

#endif//OS_FUNCTIONAL

#endif

/* -------------------------------------------------------------------------- */

typedef uint8_t               lck_t;
typedef uint8_t               stk_t;

/* -------------------------------------------------------------------------- */

extern  stk_t               __stack[];
#define MAIN_TOP            __stack+1

/* -------------------------------------------------------------------------- */
// task context

typedef struct __ctx ctx_t;

struct __ctx
{
#if defined(__AVR_TINY__)
	char     regs[2];  // call-saved registers (r18-r19)
#else
	char     regs[16]; // call-saved registers (r2-r17)
#endif
	unsigned fp;       // frame pointer        (r29:r28)
	void   * sp;       // stack pointer        (SPH:SPL)
	char     sreg;     // status register      (SREG)
	fun_t  * pc;       // return address       (PC)
#if defined(__AVR_3_BYTE_PC__) && __AVR_3_BYTE_PC__
	char     pc_;      // high part of return address
#endif
};

#if defined(__AVR_3_BYTE_PC__) && __AVR_3_BYTE_PC__
#define _CTX_INIT() { { 0 }, 0, 0, 0, 0, 0 }
#else
#define _CTX_INIT() { { 0 }, 0, 0, 0, 0 }
#endif

/* -------------------------------------------------------------------------- */
// init task context

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, stk_t *sp, fun_t *pc )
{
	ctx->sp = sp - 1;
	ctx->pc = pc;
}

/* -------------------------------------------------------------------------- */
// get current stack pointer

__STATIC_INLINE
void * port_get_sp( void )
{
	return (void *) SP;
}

/* -------------------------------------------------------------------------- */

#define port_get_lock()       SREG
#define port_put_lock(lck)    SREG = lck

#define port_set_lock()       cli()
#define port_clr_lock()       sei()

#define port_set_barrier()    asm volatile ("nop" ::: "memory")

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSCORE_H
