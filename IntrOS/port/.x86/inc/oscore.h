/******************************************************************************

    @file    IntrOS: oscore.h
    @author  Rajmund Szymanski
    @date    22.08.2018
    @brief   IntrOS port file for X86.

 ******************************************************************************

    IntrOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of IntrOS distribution.

    IntrOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    IntrOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef __INTROSCORE_H
#define __INTROSCORE_H

#include "osbase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_STACK_SIZE
#define OS_STACK_SIZE     16384 /* default task stack size in bytes           */
#endif

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#ifndef OS_FUNCTIONAL

#define OS_FUNCTIONAL         1 /* include c++ functional library header      */

#endif//OS_FUNCTIONAL

#endif

/* -------------------------------------------------------------------------- */

#ifdef __x86_64__
typedef uint64_t              lck_t;
typedef uint64_t              stk_t;
#else
typedef uint32_t              lck_t;
typedef uint32_t              stk_t;
#endif

/* -------------------------------------------------------------------------- */
// task context

typedef struct __ctx ctx_t;

struct __ctx
{
#ifdef __x86_64
#ifdef __linux__
	long long int Rbx;
	long long int Rbp;
	long long int R12;
	long long int R13;
	long long int R14;
	long long int R15;
	long long int Rsp;
	long long int Rip;
#else
	long long int Frame;
	long long int Rbx;
	long long int Rsp;
	long long int Rbp;
	long long int Rsi;
	long long int Rdi;
	long long int R12;
	long long int R13;
	long long int R14;
	long long int R15;
	long long int Rip;
	long long int Spare;
#endif
#else
#ifdef __linux__
	unsigned long Ebx;
	unsigned long Esi;
	unsigned long Edi;
	unsigned long Ebp;
	unsigned long Esp;
	unsigned long Eip;
#else
	unsigned long Ebp;
	unsigned long Ebx;
	unsigned long Edi;
	unsigned long Esi;
	unsigned long Esp;
	unsigned long Eip;
#endif
#endif
};

#ifdef __x86_64__
#ifdef __linux__
#define _CTX_INIT() { 0, 0, 0, 0, 0, 0, 0, 0 }
#else
#define _CTX_INIT() { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#endif
#else
#define _CTX_INIT() { 0, 0, 0, 0, 0, 0 }
#endif

/* -------------------------------------------------------------------------- */
// init task context

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, stk_t *sp, fun_t *pc )
{
	setjmp((void *)ctx);
#ifdef __x86_64__
	ctx->Rsp = (uintptr_t) sp;
	ctx->Rip = (uintptr_t) pc;
#else
	ctx->Esp = (uintptr_t) sp;
	ctx->Eip = (uintptr_t) pc;
#endif
}

/* -------------------------------------------------------------------------- */
// get current stack pointer

__STATIC_INLINE
void * port_get_sp( void )
{
	void *sp;
#ifdef __x86_64__
	__ASM volatile ("mov %%rsp, %0" : "=&r" (sp));
#else
	__ASM volatile ("mov %%esp, %0" : "=&r" (sp));
#endif
	return sp;
}

/* -------------------------------------------------------------------------- */

__STATIC_FORCEINLINE
lck_t __get_cc( void )
{
	lck_t cc;
	__ASM volatile ("pushf; pop %0" : "=&r" (cc));
	return cc;
}

__STATIC_FORCEINLINE
void __set_cc( lck_t cc )
{
	__ASM volatile ("push %0; popf" :: "r" (cc) : "cc");
}

__STATIC_FORCEINLINE
void __cli( void )
{
	__ASM volatile ("cli" ::: "cc");
}

__STATIC_FORCEINLINE
void __sti( void )
{
	__ASM volatile ("sti" ::: "cc");
}

__STATIC_FORCEINLINE
void __nop( void )
{
	__ASM volatile ("nop" ::: "memory");
}

__STATIC_FORCEINLINE
unsigned __xchg( volatile unsigned *lock, unsigned value )
{
	__ASM volatile ("lock xchg %0, %1" : "+r" (value) : "m" (*lock) : "memory");
	return value;
}

/* -------------------------------------------------------------------------- */

#define port_get_lock()      (lck_t)(0)
#define port_put_lock(cc)    (void)(cc)

#define port_set_lock()      (void)(0)
#define port_clr_lock()      (void)(0)

#define port_set_barrier()   (void)(0)

/* -------------------------------------------------------------------------- */

#ifdef  OS_MULTICORE

#error  OS_MULTICORE is an internal port definition!

#else

#define OS_MULTICORE

__STATIC_INLINE
void port_spn_lock( volatile unsigned *lock )
{
	while (*lock || __xchg(lock, 1));
}

#endif//OS_MULTICORE

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSCORE_H
