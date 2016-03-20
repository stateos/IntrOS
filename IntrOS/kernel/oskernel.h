/******************************************************************************

    @file    IntrOS: oskernel.h
    @author  Rajmund Szymanski
    @date    20.03.2016
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

#pragma once

#include <stdlib.h>
#include <bitband.h>
#include <osbase.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern sys_t System; // system data

/* -------------------------------------------------------------------------- */

// initiating and running the system timer
void port_sys_init( void );

/* -------------------------------------------------------------------------- */

// save status of current process and force yield system control to the next
void core_ctx_switch( void );

/* -------------------------------------------------------------------------- */

// abort and reset current process and force yield system control to the next
void core_tsk_break( void ) __noreturn;

/* -------------------------------------------------------------------------- */

// insert object 'obj' into tasks/timers queue before the 'nxt' object
// set object id to 'id'
void core_rdy_insert( os_id obj, unsigned id, os_id nxt );

// remove object 'obj' from tasks/timers queue
// set object id to ID_STOPPED
void core_rdy_remove( os_id obj );

/* -------------------------------------------------------------------------- */

// add timer 'tmr' to tasks/timers queue
// start countdown
static inline
void core_tmr_insert( tmr_id tmr ) { core_rdy_insert(tmr, ID_TIMER, Current); }

// remove timer 'tmr' from tasks/timers queue
static inline
void core_tmr_remove( tmr_id tmr ) { core_rdy_remove(tmr); }

/* -------------------------------------------------------------------------- */

// add task 'tsk' to tasks/timers queue with id ID_READY
static inline
void core_tsk_insert( tsk_id tsk ) { core_rdy_insert(tsk, ID_READY, Current); }

// remove task 'tsk' from tasks/timers queue
static inline
void   core_tsk_remove( tsk_id tsk ) { core_rdy_remove(tsk); }

// tasks queue handler procedure
// return a pointer to next READY task
tsk_id core_tsk_handler( void );

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
