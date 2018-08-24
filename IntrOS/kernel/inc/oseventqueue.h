/******************************************************************************

    @file    IntrOS: oseventqueue.h
    @author  Rajmund Szymanski
    @date    24.08.2018
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

#ifndef __INTROS_EVT_H
#define __INTROS_EVT_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : event queue
 *
 ******************************************************************************/

typedef struct __evt evt_t, * const evt_id;

struct __evt
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	unsigned*data;  // data buffer
};

/******************************************************************************
 *
 * Name              : _EVT_INIT
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

#define               _EVT_INIT( _limit, _data ) { 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _EVT_DATA
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
#define               _EVT_DATA( _limit ) (unsigned[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_EVT
 *
 * Description       : define and initialize an event queue object
 *
 * Parameters
 *   evt             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define             OS_EVT( evt, limit )                                \
                       unsigned evt##__buf[limit];                       \
                       evt_t evt##__evt = _EVT_INIT( limit, evt##__buf ); \
                       evt_id evt = & evt##__evt

/******************************************************************************
 *
 * Name              : static_EVT
 *
 * Description       : define and initialize a static event queue object
 *
 * Parameters
 *   evt             : name of a pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

#define         static_EVT( evt, limit )                                \
                static unsigned evt##__buf[limit];                       \
                static evt_t evt##__evt = _EVT_INIT( limit, evt##__buf ); \
                static evt_id evt = & evt##__evt

/******************************************************************************
 *
 * Name              : EVT_INIT
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
#define                EVT_INIT( limit ) \
                      _EVT_INIT( limit, _EVT_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : EVT_CREATE
 * Alias             : EVT_NEW
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
#define                EVT_CREATE( limit ) \
           (evt_t[]) { EVT_INIT  ( limit ) }
#define                EVT_NEW \
                       EVT_CREATE
#endif

/******************************************************************************
 *
 * Name              : evt_init
 *
 * Description       : initialize an event queue object
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   limit           : size of a queue (max number of stored events)
 *   data            : event queue data buffer
 *
 * Return            : none
 *
 ******************************************************************************/

void evt_init( evt_t *evt, unsigned limit, unsigned *data );

/******************************************************************************
 *
 * Name              : evt_wait
 *
 * Description       : try to transfer event data from the event queue object,
 *                     wait indefinitely while the event queue object is empty
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : pointer to store event data
 *
 * Return            : none
 *
 ******************************************************************************/

void evt_wait( evt_t *evt, unsigned *data );

/******************************************************************************
 *
 * Name              : evt_take
 *
 * Description       : try to transfer event data from the event queue object,
 *                     don't wait if the event queue object is empty
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : pointer to store event data
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered from the event queue object
 *   E_FAILURE       : event queue object is empty
 *
 ******************************************************************************/

unsigned evt_take( evt_t *evt, unsigned *data );

/******************************************************************************
 *
 * Name              : evt_send
 *
 * Description       : try to transfer event data to the event queue object,
 *                     wait indefinitely while the event queue object is full
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event value
 *
 * Return            : none
 *
 ******************************************************************************/

void evt_send( evt_t *evt, unsigned data );

/******************************************************************************
 *
 * Name              : evt_give
 *
 * Description       : try to transfer event data to the event queue object,
 *                     don't wait if the event queue object is full
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event value
 *
 * Return
 *   E_SUCCESS       : event data was successfully transfered to the event queue object
 *   E_FAILURE       : event queue object is full
 *
 ******************************************************************************/

unsigned evt_give( evt_t *evt, unsigned data );

/******************************************************************************
 *
 * Name              : evt_push
 *
 * Description       : transfer event data to the event queue object,
 *                     remove the oldest event data if the event queue object is full
 *
 * Parameters
 *   evt             : pointer to event queue object
 *   data            : event value
 *
 * Return            : none
 *
 ******************************************************************************/

void evt_push( evt_t *evt, unsigned data );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : EventQueueT<>
 *
 * Description       : create and initialize an event queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored events)
 *
 ******************************************************************************/

template<unsigned limit_>
struct EventQueueT : public __evt
{
	EventQueueT( void ): __evt _EVT_INIT(limit_, data_) {}

	void     wait( unsigned *_data ) {        evt_wait(this, _data); }
	unsigned take( unsigned *_data ) { return evt_take(this, _data); }
	void     send( unsigned  _data ) {        evt_send(this, _data); }
	unsigned give( unsigned  _data ) { return evt_give(this, _data); }
	void     push( unsigned  _data ) {        evt_push(this, _data); }

	private:
	unsigned data_[limit_];
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_EVT_H
