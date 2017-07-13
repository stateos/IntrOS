/******************************************************************************

    @file    IntrOS: osbase.h
    @author  Rajmund Szymanski
    @date    12.07.2017
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

/* -------------------------------------------------------------------------- */

#if defined(__cplusplus) && (OS_FUNCTIONAL)
#include <functional>
typedef std::function<void()> FUN_t;
#else
typedef               void (* FUN_t)( void );
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

typedef struct __tmr tmr_t, * const tmr_id; // timer
typedef struct __tsk tsk_t, * const tsk_id; // task
typedef         void fun_t(); // timer/task procedure

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
#if OS_TIMER == 0
	volatile
	uint32_t    cnt;   // system timer counter
#endif
};

/* -------------------------------------------------------------------------- */

#define USEC       (uint32_t)((OS_FREQUENCY)/1000000.0+0.5)
#define MSEC       (uint32_t)((OS_FREQUENCY)/   1000.0+0.5)
#define  SEC       (uint32_t)((OS_FREQUENCY)*      1.0+0.5)
#define  MIN       (uint32_t)((OS_FREQUENCY)*     60.0+0.5)
#define HOUR       (uint32_t)((OS_FREQUENCY)*   3600.0+0.5)

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
#define IMMEDIATE  (uint32_t)( 0UL) // no waiting
#endif
#ifndef INFINITE
#define INFINITE   (uint32_t)(~0UL) // infinite waiting
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSBASE_H
