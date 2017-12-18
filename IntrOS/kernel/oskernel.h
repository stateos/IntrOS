/******************************************************************************

    @file    IntrOS: oskernel.h
    @author  Rajmund Szymanski
    @date    18.12.2017
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

#include <string.h>
#include <stdlib.h>
#include <oscore.h>

/* -------------------------------------------------------------------------- */

#ifndef DEBUG
#ifndef NDEBUG
#define NDEBUG
#endif//NDEBUG
#endif//DEBUG

#include <assert.h>

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#if OS_FUNCTIONAL
#include <functional>
typedef std::function<void( void )> FUN_t;
#else
typedef     void (* FUN_t)( void );
#endif

#endif

/* -------------------------------------------------------------------------- */

#define ALIGNED( size, type ) \
     (((size_t)( size )+sizeof(type)-1)&~(sizeof(type)-1))

#define LIMITED( size, type ) \
     (((size_t)( size ))&~(sizeof(type)-1))

#define ALIGNED_SIZE( size, type ) \
          (((size_t)( size )+sizeof(type)-1)/sizeof(type))

#define LIMITED_SIZE( size, type ) \
          (((size_t)( size ))/sizeof(type))

#define ABOVE( size ) \
      ALIGNED( size, stk_t )

#define SSIZE( size ) \
 ALIGNED_SIZE( size, stk_t )

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern tsk_t MAIN;   // main task
extern sys_t System; // system data

/* -------------------------------------------------------------------------- */

#define core_stk_assert() \
        assert((System.cur == &MAIN) || (System.cur->id == ID_TIMER) || (System.cur->stack <= port_get_sp()))

/* -------------------------------------------------------------------------- */

// initiate and run the system timer
// the port_sys_init procedure is normally called as a constructor
__CONSTRUCTOR
void port_sys_init( void );

/* -------------------------------------------------------------------------- */

// initiate task 'tsk' for context switch
void core_ctx_init( tsk_t *tsk );

// save status of the current process and force yield system control to the next
void core_ctx_switch( void );

// system infinite loop procedure for the current process
__NO_RETURN
void core_tsk_loop( void );

// force yield system control to the next process
__NO_RETURN
void core_tsk_switch( void );

/* -------------------------------------------------------------------------- */

// insert object 'obj' into tasks/timers queue before the 'nxt' object
// set object id to 'id'
void core_rdy_insert( void *obj, unsigned id, void *nxt );

// remove object 'obj' from tasks/timers queue
// set object id to ID_STOPPED
void core_rdy_remove( void *obj );

/* -------------------------------------------------------------------------- */

// insert timer 'tmr' into tasks/timers queue and start it
__STATIC_INLINE
void core_tmr_insert( tmr_t *tmr ) { core_rdy_insert(tmr, ID_TIMER, System.cur); }

// remove timer 'tmr' from tasks/timers queue
__STATIC_INLINE
void core_tmr_remove( tmr_t *tmr ) { core_rdy_remove(tmr); }

/* -------------------------------------------------------------------------- */

// insert task 'tsk' into tasks/timers queue with id ID_READY
__STATIC_INLINE
void core_tsk_insert( tsk_t *tsk ) { core_rdy_insert(tsk, ID_READY, System.cur); }

// remove task 'tsk' from tasks/timers queue
__STATIC_INLINE
void core_tsk_remove( tsk_t *tsk ) { core_rdy_remove(tsk); }

/* -------------------------------------------------------------------------- */

// return current system time
__STATIC_INLINE
uint32_t core_sys_time( void )
{
#if OS_TICKLESS
	return System.cnt + port_sys_time();
#else
	return System.cnt;
#endif
}

// internal handler of system timer
void core_sys_tick( void );

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSKERNEL_H
