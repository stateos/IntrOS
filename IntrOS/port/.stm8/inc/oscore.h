/******************************************************************************

    @file    IntrOS: oscore.h
    @author  Rajmund Szymanski
    @date    16.07.2018
    @brief   IntrOS port file for STM8 uC.

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
#define OS_STACK_SIZE       128 /* default task stack size in bytes           */
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

#if   defined(__CSMC__)
extern  stk_t                _stack[];
#define MAIN_TOP             _stack+1
#endif

/* -------------------------------------------------------------------------- */
// task context

typedef struct __ctx ctx_t;

#if   defined(__CSMC__)

struct __ctx
{
	void   * sp;
	fun_t  * pc;
};

#elif defined(__SDCC)

struct __ctx
{
	fun_t  * pc;
	void   * sp;
};

#endif

#define _CTX_INIT() { 0, 0 }

/* -------------------------------------------------------------------------- */
// init task context

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, stk_t *sp, fun_t *pc )
{
	ctx->sp = sp - 1;
	ctx->pc = pc;
}

/* -------------------------------------------------------------------------- */

#if   defined(__CSMC__)

#define _get_SP()    (void *)_asm("ldw x, sp")
#define _set_SP(sp)  (void)  _asm("ldw sp, x", (void *)(sp))

#define _get_CC()    (lck_t) _asm("push cc""\n""pop a")
#define _set_CC(cc)  (void)  _asm("push a""\n""pop cc", (lck_t)(cc))

#elif defined(__SDCC)

void  * _get_SP( void );
void    _set_SP( void *sp );

lck_t   _get_CC( void );
void    _set_CC( lck_t cc );

#endif

/* -------------------------------------------------------------------------- */
// get current stack pointer

__STATIC_INLINE
void * port_get_sp( void )
{
	return _get_SP();
}

/* -------------------------------------------------------------------------- */

#define port_get_lock()      _get_CC()
#define port_put_lock(lck)   _set_CC(lck)

#define port_set_lock()       disableInterrupts()
#define port_clr_lock()       enableInterrupts()

#define port_set_barrier()    nop()

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSCORE_H
