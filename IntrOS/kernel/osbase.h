/******************************************************************************

    @file    IntrOS: osbase.h
    @author  Rajmund Szymanski
    @date    10.03.2017
    @brief   This file contains basic definitions for IntrOS.

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

#ifndef __INTROSBASE_H
#define __INTROSBASE_H

#include <stdbool.h>
#include <setjmp.h>
#include <osport.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define USEC       (unsigned)(OS_FREQUENCY)/1000000U
#define MSEC       (unsigned)(OS_FREQUENCY)/1000U
#define  SEC       (unsigned)(OS_FREQUENCY)
#define  MIN       (unsigned)(OS_FREQUENCY)*60U
#define HOUR       (unsigned)(OS_FREQUENCY)*3600U

/* -------------------------------------------------------------------------- */

#define ID_STOPPED ( 0U) // task or timer stopped
#define ID_READY   ( 1U) // task ready to run
#define ID_DELAYED ( 2U) // task in the delayed state
#define ID_TIMER   ( 3U) // timer in the countdown state

/* -------------------------------------------------------------------------- */

#define E_SUCCESS  ( 0U) // process released by taking the supervising object
#define E_FAILURE  (~0U)

/* -------------------------------------------------------------------------- */

#ifndef IMMEDIATE
#define IMMEDIATE  ( 0U) // no waiting
#endif
#ifndef INFINITE
#define INFINITE   (~0U) // infinite waiting
#endif

/* -------------------------------------------------------------------------- */

#define ASIZE( size ) \
 (((unsigned)( size )+(sizeof(stk_t)-1))/sizeof(stk_t))

/* -------------------------------------------------------------------------- */

typedef struct __tmr tmr_t, *tmr_id; // timer
typedef struct __tsk tsk_t, *tsk_id; // task

typedef void fun_t(); // timer/task procedure

/* -------------------------------------------------------------------------- */

// queue

typedef struct __que que_t;

struct __que
{
	que_t  * next; // next object in the queue
};

/* -------------------------------------------------------------------------- */

// object (timer, task) header

typedef struct __obj obj_t;

struct __obj
{
	unsigned id;    // object id: ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER
	void   * prev;  // previous object in the READY queue
	void   * next;  // next object in the READY queue
};

/* -------------------------------------------------------------------------- */

// system data

typedef struct __sys sys_t;

struct __sys
{
	tsk_t  * cur;   // pointer to the current task control block
	volatile
	unsigned cnt;   // system timer counter
};

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

#endif

#if __FPU_USED
#define _CTX_INIT() { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 } }
#else
#define _CTX_INIT() { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#endif

/* -------------------------------------------------------------------------- */

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, stk_t *sp, fun_t *pc )
{
	ctx->sp = sp;
	ctx->pc = pc;
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSBASE_H
