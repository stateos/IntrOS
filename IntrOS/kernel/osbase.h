/******************************************************************************

    @file    IntrOS: osbase.h
    @author  Rajmund Szymanski
    @date    11.01.2017
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

#define IMMEDIATE  ( 0U) // no waiting
#define INFINITE   (~0U) // infinite waiting

/* -------------------------------------------------------------------------- */

#define ASIZE( size ) \
 (((unsigned)( size )+(sizeof(stk_t)-1))/sizeof(stk_t))

/* -------------------------------------------------------------------------- */

typedef   void (*fun_id)(); // task/timer procedure
typedef struct __que que_t; // queue
typedef struct __tmr tmr_t; // timer
typedef struct __tsk tsk_t; // task

/* -------------------------------------------------------------------------- */

// queue

struct __que
{
	que_t  * next;
};

/* -------------------------------------------------------------------------- */

// object (timer, task) header

typedef struct __obj
{
	unsigned id;    // object id: ID_STOPPED, ID_READY, ID_DELAYED, ID_TIMER
	void   * prev;  // previous object in the READY queue
	void   * next;  // next object in the READY queue

}	obj_t;

/* -------------------------------------------------------------------------- */

// system data

typedef struct __sys
{
	tsk_t  * cur;   // pointer to the current task control block
	volatile
	unsigned cnt;   // system timer counter

}	sys_t;

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSBASE_H
