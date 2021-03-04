/******************************************************************************

    @file    IntrOS: osevent.h
    @author  Rajmund Szymanski
    @date    02.03.2021
    @brief   This file contains definitions for IntrOS.

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

#ifndef __INTROS_EVT_H
#define __INTROS_EVT_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : event
 *
 ******************************************************************************/

typedef struct __evt evt_t, * const evt_id;

struct __evt
{
	unsigned event;

	unsigned signal;
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _EVT_INIT
 *
 * Description       : create and initialize an event object
 *
 * Parameters        : none
 *
 * Return            : event object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _EVT_INIT() { 0, 0 }

/******************************************************************************
 *
 * Name              : OS_EVT
 *
 * Description       : define and initialize an event object
 *
 * Parameters
 *   evt             : name of a pointer to event object
 *
 ******************************************************************************/

#define             OS_EVT( evt )                     \
                       evt_t evt##__evt = _EVT_INIT(); \
                       evt_id evt = & evt##__evt

/******************************************************************************
 *
 * Name              : static_EVT
 *
 * Description       : define and initialize a static event object
 *
 * Parameters
 *   evt             : name of a pointer to event object
 *
 ******************************************************************************/

#define         static_EVT( evt )                     \
                static evt_t evt##__evt = _EVT_INIT(); \
                static evt_id evt = & evt##__evt

/******************************************************************************
 *
 * Name              : EVT_INIT
 *
 * Description       : create and initialize an event object
 *
 * Parameters        : none
 *
 * Return            : event object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVT_INIT() \
                      _EVT_INIT()
#endif

/******************************************************************************
 *
 * Name              : EVT_CREATE
 * Alias             : EVT_NEW
 *
 * Description       : create and initialize an event object
 *
 * Parameters        : none
 *
 * Return            : pointer to event object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVT_CREATE() \
           (evt_t[]) { EVT_INIT  () }
#define                EVT_NEW \
                       EVT_CREATE
#endif

/******************************************************************************
 *
 * Name              : evt_init
 *
 * Description       : initialize an event object
 *
 * Parameters
 *   evt             : pointer to event object
 *
 * Return            : none
 *
 ******************************************************************************/

void evt_init( evt_t *evt );

/******************************************************************************
 *
 * Name              : evt_take
 *
 * Description       : get last event from the event object
 *
 * Parameters
 *   evt             : pointer to event object
 *
 * Return            : event value
 *
 ******************************************************************************/

unsigned evt_take( evt_t *evt );

/******************************************************************************
 *
 * Name              : evt_wait
 *
 * Description       : wait indefinitely until the event object has been released
 *
 * Parameters
 *   evt             : pointer to event object
 *
 * Return            : event value
 *
 ******************************************************************************/

unsigned evt_wait( evt_t *evt );

/******************************************************************************
 *
 * Name              : evt_give
 *
 * Description       : resume all tasks that are waiting on the event object
 *
 * Parameters
 *   evt             : pointer to event object
 *   event           : event value
 *
 * Return            : none
 *
 ******************************************************************************/

void evt_give( evt_t *evt, unsigned event );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : Event
 *
 * Description       : create and initialize an event object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct Event : public __evt
{
	constexpr
	Event( void ): __evt _EVT_INIT() {}

	Event( Event&& ) = default;
	Event( const Event& ) = delete;
	Event& operator=( Event&& ) = delete;
	Event& operator=( const Event& ) = delete;

	unsigned take( void )            { return evt_take(this); }
	unsigned wait( void )            { return evt_wait(this); }
	void     give( unsigned _event ) {        evt_give(this, _event); }
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_EVT_H
