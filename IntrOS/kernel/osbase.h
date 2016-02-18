/******************************************************************************

    @file    IntrOS: osbase.h
    @author  Rajmund Szymanski
    @date    18.02.2016
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

#pragma once

#include <stdbool.h>
#include <osport.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CC_ARM
#pragma push
#pragma anon_unions
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
#define E_FAILURE  (!E_SUCCESS)

/* -------------------------------------------------------------------------- */

#define IMMEDIATE  ( 0U) // no waiting
#define INFINITE   (~0U) // infinite waiting

/* -------------------------------------------------------------------------- */

#define __osalign \
        __attribute__((aligned(OS_ALIGN)))

/* -------------------------------------------------------------------------- */

#define ASIZE( size ) \
        ((size + OS_ALIGN - 1) & ~(OS_ALIGN - 1))

/* -------------------------------------------------------------------------- */

typedef void                 *os_id;
typedef void               (*fun_id)(void);
typedef struct __sig sig_t, *sig_id; // signal
typedef struct __evt evt_t, *evt_id; // event
typedef struct __flg flg_t, *flg_id; // flag
typedef struct __bar bar_t, *bar_id; // barrier
typedef struct __sem sem_t, *sem_id; // semaphore
typedef struct __mtx mtx_t, *mtx_id; // mutex
typedef struct __cnd cnd_t, *cnd_id; // condition variable
typedef struct __box box_t, *box_id; // mailbox queue
typedef struct __msg msg_t, *msg_id; // message queue
typedef struct __obj obj_t, *obj_id;
typedef struct __tmr tmr_t, *tmr_id; // timer
typedef struct __tsk tsk_t, *tsk_id; // task
typedef struct __sys sys_t, *sys_id; // system data

/* -------------------------------------------------------------------------- */

// signal

struct __sig
{
	unsigned flag;    // signal's current value
	unsigned protect; // signal type: sigClear, sigProtect
};

#define  sigClear    ( 0U )
#define  sigProtect  ( 1U )
#define  sigMASK     ( 1U )

#define _SIG_INIT( protect ) { 0, (protect)&sigMASK }

/* -------------------------------------------------------------------------- */

// event

struct __evt
{
	unsigned signal;
	unsigned event;
};

#define _EVT_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// flag

struct __flg
{
	unsigned flags; // flag's current value
};

#define  flgOne  ( false )
#define  flgAll  ( true  )

#define _FLG_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// barrier

struct __bar
{
	unsigned signal;
	unsigned count; // barrier's current value
	unsigned limit; // barrier's value limit
};

#define _BAR_INIT( limit ) { 0, limit, limit }

/* -------------------------------------------------------------------------- */

// semaphore

struct __sem
{
	unsigned count; // semaphore's current value
};

#define _SEM_INIT( count ) { count }

/* -------------------------------------------------------------------------- */

// mutex

struct __mtx
{
	tsk_id   owner; // owner task
};

#define _MTX_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// condition variable

struct __cnd
{
	unsigned signal;
};

#define _CND_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// mailbox queue

struct __box
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	char    *data;  // queue data
	unsigned size;  // size of a single mail (in bytes)
};

#define _BOX_INIT( limit, size, data ) { 0, limit, 0, 0, data, size }

#define _BOX_DATA( limit, size ) (char[limit*size]){ 0 }

/* -------------------------------------------------------------------------- */

// message queue

struct __msg
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	unsigned*data;  // queue data
};

#define _MSG_INIT( limit, data ) { 0, limit, 0, 0, data }

#define _MSG_DATA( limit ) (unsigned[limit]){ 0 }

/* -------------------------------------------------------------------------- */

// object header (timer, task)

struct __obj
{
	unsigned id;    // object id: ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER
	obj_id   next;  // next object in the READY queue
	obj_id   prev;  // previous object in the READY queue
};

/* -------------------------------------------------------------------------- */

// timer

struct __tmr
{
	unsigned id;    // inherited from object
	tmr_id   next;  // inherited from object
	tmr_id   prev;  // inherited from object
	unsigned signal;

	fun_id   state; // callback procedure
	unsigned start;
	unsigned delay;
	unsigned period;
};

#define _TMR_INIT() { 0 }

/* -------------------------------------------------------------------------- */

// task

struct __tsk
{
	unsigned id;    // inherited from timer
	tsk_id   next;  // inherited from timer
	tsk_id   prev;  // inherited from timer
	unsigned event; // wakeup event

	fun_id   state; // inherited from timer
	unsigned start; // inherited from timer
	unsigned delay; // inherited from timer
	unsigned period;// inherited from timer

	void    *sp;    // stack pointer
	void    *top;   // top of stack
};

#define _TSK_INIT( state, top ) { 0, 0, 0, 0, state, 0, 0, 0, 0, top }

#define _TSK_STACK( size ) (__osalign char[ASIZE(size)]){ 0 } + ASIZE(size)

/* -------------------------------------------------------------------------- */

// system data

struct __sys
{
	tsk_id   cur;   // pointer to the current task control block
	volatile
	unsigned cnt;   // system timer counter
};

/* -------------------------------------------------------------------------- */

#ifdef __CC_ARM
#pragma pop
#endif

#ifdef __cplusplus
}
#endif
