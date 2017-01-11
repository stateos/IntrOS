/******************************************************************************

    @file    IntrOS: oskernel.h
    @author  Rajmund Szymanski
    @date    11.01.2017
    @brief   This file defines set of kernel functions for IntrOS.

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

#ifndef __INTROSKERNEL_H
#define __INTROSKERNEL_H

#include <stdlib.h>
#include <osbase.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern sys_t System; // system data

/* -------------------------------------------------------------------------- */

// initiating and running the system timer
// the port_sys_init procedure is normally called as a constructor
void port_sys_init( void ) __CONSTRUCTOR;

/* -------------------------------------------------------------------------- */

// save status of current process and force yield system control to the next
void core_ctx_switch( void );

/* -------------------------------------------------------------------------- */

// abort and reset current process and force yield system control to the next
void core_tsk_break( void ) __NORETURN;

/* -------------------------------------------------------------------------- */

// insert object 'obj' into tasks/timers queue before the 'nxt' object
// set object id to 'id'
void core_rdy_insert( void *obj, unsigned id, void *nxt );

// remove object 'obj' from tasks/timers queue
// set object id to ID_STOPPED
void core_rdy_remove( void *obj );

/* -------------------------------------------------------------------------- */

// add timer 'tmr' to tasks/timers queue
// start countdown
static inline
void core_tmr_insert( tmr_t *tmr ) { core_rdy_insert(tmr, ID_TIMER, Current); }

// remove timer 'tmr' from tasks/timers queue
static inline
void core_tmr_remove( tmr_t *tmr ) { core_rdy_remove(tmr); }

/* -------------------------------------------------------------------------- */

// add task 'tsk' to tasks/timers queue with id ID_READY
static inline
void core_tsk_insert( tsk_t *tsk ) { core_rdy_insert(tsk, ID_READY, Current); }

// remove task 'tsk' from tasks/timers queue
static inline
void   core_tsk_remove( tsk_t *tsk ) { core_rdy_remove(tsk); }

// tasks queue handler procedure
// return a pointer to next READY task
tsk_t *core_tsk_handler( void );

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROSKERNEL_H
