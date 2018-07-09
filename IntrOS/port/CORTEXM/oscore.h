/******************************************************************************

    @file    IntrOS: oscore.h
    @author  Rajmund Szymanski
    @date    09.07.2018
    @brief   IntrOS port file for ARM Cotrex-M uC.

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

#include <osbase.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_STACK_SIZE
#define OS_STACK_SIZE       256 /* default task stack size in bytes           */
#endif

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#ifndef OS_FUNCTIONAL

#if   defined(__CC_ARM) || defined(__CSMC__) || defined(__ICCARM__)
#define OS_FUNCTIONAL         0 /* c++ functional library header not included */
#else
#define OS_FUNCTIONAL         1 /* include c++ functional library header      */
#endif

#elif   OS_FUNCTIONAL

#if   defined(__CC_ARM) || defined(__CSMC__) || defined(__ICCARM__)
#error  c++ functional library not allowed for this compiler.
#endif

#endif//OS_FUNCTIONAL

#endif

/* -------------------------------------------------------------------------- */

typedef uint32_t              lck_t;
typedef uint64_t              stk_t;

/* -------------------------------------------------------------------------- */

extern  stk_t               __initial_sp[];
#define MAIN_TOP            __initial_sp

/* -------------------------------------------------------------------------- */
// task context

typedef struct __ctx ctx_t;

#if   defined(__ARMCC_VERSION)

struct __ctx
{
	unsigned r8, r9, r10, r11;
	fun_t  * pc;
	unsigned r4, r5, r6, r7;
	void   * sp;
#if __FPU_USED
	float    s[16]; // s16-s31
#endif
};

#elif defined(__GNUC__)

struct __ctx
{
	unsigned r4, r5, r6, r7, r8, r9, r10, r11;
	void   * sp;
	fun_t  * pc;
#if __FPU_USED
	float    s[16]; // s16-s31
#endif
};

#elif defined(__CSMC__)

struct __ctx
{
	unsigned r4, r5, r6, r7, r8, r9, r10, r11;
	fun_t  * pc;
	void   * sp;
#if __FPU_USED
	float    s[16]; // s16-s31
#endif
};

#elif defined(__ICCARM__)

struct __ctx
{
	unsigned r4, r5, r6, r7, r8, r9, r10, r11;
	void   * sp;
	fun_t  * pc;
#if __FPU_USED
	float    s[16]; // s16-s31
#endif
};

#endif

#if __FPU_USED
#define _CTX_INIT() { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 } }
#else
#define _CTX_INIT() { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#endif

/* -------------------------------------------------------------------------- */
// init task context

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, stk_t *sp, fun_t *pc )
{
	ctx->sp = sp;
	ctx->pc = pc;
}

/* -------------------------------------------------------------------------- */
// get current stack pointer

__STATIC_INLINE
void * port_get_sp( void )
{
	uint32_t sp;
#if   defined(__CC_ARM)
	sp = __current_sp();
#elif defined(__CSMC__)
	sp = __ASM ("mov r0, sp");
#elif defined(__ICCARM__)
	sp = __get_SP();
#else
	__ASM volatile ("mov %0, sp" : "=r" (sp));
#endif
	return (void *) sp;
}

/* -------------------------------------------------------------------------- */

#if   defined(__CSMC__)

#define __disable_irq()     __ASM("cpsid i")
#define __enable_irq()      __ASM("cpsie i")

#endif

/* -------------------------------------------------------------------------- */

#define port_get_lock()     __get_PRIMASK()
#define port_put_lock(lck)  __set_PRIMASK(lck)

#define port_set_lock()     __disable_irq()
#define port_clr_lock()     __enable_irq()

#define port_sys_lock()  do { lck_t __LOCK = port_get_lock(); port_set_lock()
#define port_sys_unlock()     port_put_lock(__LOCK); } while(0)

#define port_isr_lock()  do { port_set_lock()
#define port_isr_unlock()     port_clr_lock(); } while(0)

#define port_set_barrier()  __ISB()

/* -------------------------------------------------------------------------- */

#if __CORTEX_M > 0

#ifdef  OS_MULTICORE

#error  OS_MULTICORE is an internal port definition!

#else

#define OS_MULTICORE

__STATIC_INLINE
void port_spn_lock( volatile unsigned *lock )
{
    while (__LDREXW((volatile uint32_t *)lock) || __STREXW(1, (volatile uint32_t *)lock));
}

#endif//OS_MULTICORE

#endif//__CORTEX_M

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSCORE_H
