/******************************************************************************

    @file    IntrOS: os_evq.h
    @author  Rajmund Szymanski
    @date    02.05.2018
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

#ifndef __INTROS_EVQ_H
#define __INTROS_EVQ_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : event queue
 *
 ******************************************************************************/

typedef struct __evq evq_t, * const evq_id;

struct __evq
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	unsigned*data;  // queue data
};

/******************************************************************************
 *
 * Name              : _EVQ_INIT
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Return            : event queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _EVQ_INIT( _limit, _data ) { 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _EVQ_DATA
 *
 * Description       : create an event queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : event queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _EVQ_DATA( _limit ) (unsigned[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_EVQ
 *
 * Description       : define and initialize an event queue object
 *
 * Parameters
 *   evq             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define             OS_EVQ( evq, limit )                                \
                       unsigned evq##__buf[limit];                       \
                       evq_t evq##__evq = _EVQ_INIT( limit, evq##__buf ); \
                       evq_id evq = & evq##__evq

/******************************************************************************
 *
 * Name              : static_EVQ
 *
 * Description       : define and initialize a static event queue object
 *
 * Parameters
 *   evq             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define         static_EVQ( evq, limit )                                \
                static unsigned evq##__buf[limit];                       \
                static evq_t evq##__evq = _EVQ_INIT( limit, evq##__buf ); \
                static evq_id evq = & evq##__evq

/******************************************************************************
 *
 * Name              : EVQ_INIT
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : event queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVQ_INIT( limit ) \
                      _EVQ_INIT( limit, _EVQ_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : EVQ_CREATE
 * Alias             : EVQ_NEW
 *
 * Description       : create and initialize an event queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored events)
 *
 * Return            : pointer to event queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                EVQ_CREATE( limit ) \
             & (evq_t) EVQ_INIT  ( limit )
#define                EVQ_NEW \
                       EVQ_CREATE
#endif

/******************************************************************************
 *
 * Name              : evq_init
 *
 * Description       : initialize an event queue object
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Return            : none
 *
 ******************************************************************************/

void evq_init( evq_t *evq, unsigned limit, unsigned *data );

/******************************************************************************
 *
 * Name              : evq_wait
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait indefinitely while the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return
 *   'another'       : task was successfully released
 *
 ******************************************************************************/

unsigned evq_wait( evq_t *evq );

/******************************************************************************
 *
 * Name              : evq_take
 *
 * Description       : try to transfer event data from the event queue object,
 *                     don't wait if the event queue object is empty
 *
 * Parameters
 *   evq             : pointer to event queue object
 *
 * Return
 *   E_FAILURE       : event queue object is empty
 *   'another'       : task was successfully released
 *
 ******************************************************************************/

unsigned evq_take( evq_t *evq );

/******************************************************************************
 *
 * Name              : evq_send
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait indefinitely while the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return            : none
 *
 ******************************************************************************/

void evq_send( evq_t *evq, unsigned event );

/******************************************************************************
 *
 * Name              : evq_give
 *
 * Description       : try to transfer event data to the event queue object,
 *                     don't wait if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_FAILURE       : event queue object is full
 *
 ******************************************************************************/

unsigned evq_give( evq_t *evq, unsigned event );

/******************************************************************************
 *
 * Name              : evq_push
 *
 * Description       : transfer event data to the event queue object,
 *                     remove the oldest event data if the event queue object is full
 *
 * Parameters
 *   evq             : pointer to event queue object
 *   event           : event value
 *
 * Return            : none
 *
 ******************************************************************************/

void evq_push( evq_t *evq, unsigned event );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseEventQueue
 *
 * Description       : create and initialize an event queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseEventQueue : public __evq
{
	explicit
	baseEventQueue( const unsigned _limit, unsigned * const _data ): __evq _EVQ_INIT(_limit, _data) {}

	unsigned wait( void )            { return evq_wait(this);         }
	unsigned take( void )            { return evq_take(this);         }
	void     send( unsigned _event ) {        evq_send(this, _event); }
	unsigned give( unsigned _event ) { return evq_give(this, _event); }
	void     push( unsigned _event ) {        evq_push(this, _event); }
};

/******************************************************************************
 *
 * Class             : EventQueue
 *
 * Description       : create and initialize an event queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

template<unsigned _limit>
struct EventQueueT : public baseEventQueue
{
	explicit
	EventQueueT( void ): baseEventQueue(_limit, data_) {}

	private:
	unsigned data_[_limit];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_EVQ_H
