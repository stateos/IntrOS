/******************************************************************************

    @file    IntrOS: oskernel.h
    @author  Rajmund Szymanski
    @date    04.05.2020
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

/* -------------------------------------------------------------------------- *
 *                              FOR INTERNAL USE                              *
 * -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#if OS_FUNCTIONAL
#include <functional>
using Fun_t = std::function<void( void )>;
using Act_t = std::function<void( unsigned )>;
#endif

#endif

/* -------------------------------------------------------------------------- */

#define ALIGNED_SIZE( size, type ) \
          (((size_t)( size ) + sizeof(type) - 1) / sizeof(type))

#define LIMITED_SIZE( size, type ) \
          (((size_t)( size )) / sizeof(type))

#define      ALIGNED( size, type ) \
       (ALIGNED_SIZE( size, type ) * sizeof(type))

#define      LIMITED( size, type ) \
       (LIMITED_SIZE( size, type ) * sizeof(type))

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern tsk_t MAIN;   // main task
extern sys_t System; // system data

/* -------------------------------------------------------------------------- */

#define assert_ctx_integrity(tsk) \
        assert(((tsk) == &MAIN) || ((uintptr_t)(tsk)->stack < (uintptr_t)(tsk)->ctx.reg.sp))

#define assert_stk_integrity() \
        assert((System.cur == &MAIN) || ((uintptr_t)System.cur->stack < (uintptr_t)port_get_sp()))

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
	return port_sys_time();
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
