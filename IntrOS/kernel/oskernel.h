/******************************************************************************

    @file    IntrOS: oskernel.h
    @author  Rajmund Szymanski
    @date    26.12.2020
    @brief   This file defines set of kernel functions for IntrOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

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

#ifndef __INTROSKERNEL_H
#define __INTROSKERNEL_H

#include <string.h>
#include <stdlib.h>
#include "oscore.h"

#if OS_ATOMICS
#ifndef __cplusplus
#include <stdatomic.h>
#else
#include <atomic>
#endif
#endif

/* -------------------------------------------------------------------------- *
 *                              FOR INTERNAL USE                              *
 * -------------------------------------------------------------------------- */

#ifdef __cplusplus
#if    __cplusplus < 201103L
#error This version of c++ compiler is unsupported!
#endif

#if    __cplusplus >= 201402
#include <functional>
using Fun_t = std::function<void( void )>;
using Act_t = std::function<void( unsigned )>;
#endif

#endif

/* -------------------------------------------------------------------------- */

#define ALIGNED( value, alignment ) \
          (((size_t)( value ) + (size_t)( alignment ) - 1) & ~((size_t)( alignment ) - 1))

#define ALIGNED_SIZE( value, alignment ) \
          (((size_t)( value ) + (size_t)( alignment ) - 1) / (size_t)( alignment ))

#define ALIGNED_OFFSET( value, offset, alignment ) \
          (ALIGNED((size_t)( value ) + (size_t)( offset ), alignment) - (size_t)( offset ))

#define LIMITED( value, alignment ) \
          ((size_t)( value ) & ~((size_t)( alignment ) - 1))

#define LIMITED_SIZE( value, alignment ) \
          ((size_t)( value ) / (size_t)( alignment ))

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern tsk_t MAIN;   // main task
extern sys_t System; // system data

/* -------------------------------------------------------------------------- */
#ifdef DEBUG

// return high water mark of stack of the task
size_t core_stk_space( tsk_t *tsk );

// check the integrity of stack of the task while context switching
bool core_ctx_integrity( tsk_t *tsk );

// check the integrity of stack of the current task
bool core_stk_integrity( void );

#endif
/* -------------------------------------------------------------------------- */

#define assert_ctx_integrity(tsk) assert(core_ctx_integrity(tsk))

#define assert_stk_integrity()    assert(core_stk_integrity())

/* -------------------------------------------------------------------------- */

// initiate and run the system timer
// the port_sys_init procedure is normally called as a constructor
#ifdef __CONSTRUCTOR
       __CONSTRUCTOR
#endif
void port_sys_init( void );

/* -------------------------------------------------------------------------- */

// initiate task 'tsk' for context switch
void core_ctx_init( tsk_t *tsk );

// save status of the current process and force yield system control to the next
void core_ctx_switch( void );

// system infinite loop procedure for the current process
__NO_RETURN
void core_tsk_loop( void );

// system procedure for starting the current task
// this is alternative for core_tsk_loop procedure
// it executes tsk_exit while return
__NO_RETURN
void core_tsk_exec( void );

// force yield system control to the next process
__NO_RETURN
void core_tsk_switch( void );

/* -------------------------------------------------------------------------- */

// insert timer 'tmr' into tasks/timers queue and start it
void core_tmr_insert( tmr_t *tmr );

// remove timer 'tmr' from tasks/timers queue
void core_tmr_remove( tmr_t *tmr );

/* -------------------------------------------------------------------------- */

// insert task 'tsk' into tasks/timers queue with id ID_READY
void core_tsk_insert( tsk_t *tsk );

// remove task 'tsk' from tasks/timers queue
void core_tsk_remove( tsk_t *tsk );

/* -------------------------------------------------------------------------- */

// return current system time in tick-less mode
#if HW_TIMER_SIZE < OS_TIMER_SIZE // because of CSMCC
cnt_t port_sys_time( void );
#endif

// return current system time
__STATIC_INLINE
cnt_t core_sys_time( void )
{
#if HW_TIMER_SIZE == 0
	return System.cnt;
#else
	return (cnt_t)port_sys_time();
#endif
}

// internal handler of system timer
__STATIC_INLINE
void core_sys_tick( void )
{
#if HW_TIMER_SIZE < OS_TIMER_SIZE
	System.cnt += (cnt_t)(1) << (HW_TIMER_SIZE);
#endif
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__INTROSKERNEL_H
