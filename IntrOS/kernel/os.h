/******************************************************************************

    @file    IntrOS: os.h
    @author  Rajmund Szymanski
    @date    05.06.2018
    @brief   This file contains definitions for IntrOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

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

#ifndef __INTROS_H
#define __INTROS_H

#include "oskernel.h"
#include "inc/osspinlock.h"
#include "inc/ossignal.h"
#include "inc/osevent.h"
#include "inc/osflag.h"
#include "inc/osbarrier.h"
#include "inc/ossemaphore.h"
#include "inc/osmutex.h"
#include "inc/osconditionvariable.h"
#include "inc/oslist.h"
#include "inc/osmemorypool.h"
#include "inc/osstreambuffer.h"
#include "inc/osmessagebuffer.h"
#include "inc/osmailboxqueue.h"
#include "inc/osjobqueue.h"
#include "inc/oseventqueue.h"
#include "inc/ostimer.h"
#include "inc/ostask.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : sys_init
 *
 * Description       : initialize system timer and enable services
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : function port_sys_init should be invoked as a constructor
 *                   : otherwise, call sys_init as the first instruction in function main
 *
 ******************************************************************************/

__STATIC_INLINE
void sys_init( void ) { port_sys_init(); }

/******************************************************************************
 *
 * Name              : sys_lock
 *
 * Description       : disable interrupts / enter into critical section
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

#define                sys_lock() \
                       port_sys_lock()

/******************************************************************************
 *
 * Name              : sys_unlock
 *
 * Description       : enable interrupts / exit from critical section
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

#define                sys_unlock() \
                       port_sys_unlock()

/******************************************************************************
 *
 * Name              : sys_time
 *
 * Description       : return current value of system counter
 *
 * Parameters        : none
 *
 * Return            : current value of system counter
 *
 ******************************************************************************/

cnt_t sys_time( void );

/******************************************************************************
 *
 * Name              : stk_assert
 *
 * Description       : check stack integrity of the current task
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

#define                stk_assert() \
                       core_stk_assert()

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : CriticalSection
 *
 * Description       : create and initialize a critical section guard object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct CriticalSection
{
	 CriticalSection( void ) { state = port_get_lock(); port_set_lock(); }
	~CriticalSection( void ) { port_put_lock(state); }

	private:
	lck_t state;
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_H
