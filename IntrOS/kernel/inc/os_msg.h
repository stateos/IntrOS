/******************************************************************************

    @file    IntrOS: os_msg.h
    @author  Rajmund Szymanski
    @date    09.04.2018
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

#ifndef __INTROS_MSG_H
#define __INTROS_MSG_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : message queue
 *                     message is an 'unsigned int' data type; for other data types use mailbox queue
 *
 ******************************************************************************/

typedef struct __msg msg_t, * const msg_id;

struct __msg
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	unsigned*data;  // queue data
};

/******************************************************************************
 *
 * Name              : _MSG_INIT
 *
 * Description       : create and initialize a message queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *   data            : message queue data buffer
 *
 * Return            : message queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MSG_INIT( _limit, _data ) { 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _MSG_DATA
 *
 * Description       : create a message queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 * Return            : message queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _MSG_DATA( _limit ) (unsigned[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_MSG
 *
 * Description       : define and initialize a message queue object
 *
 * Parameters
 *   msg             : name of a pointer to message queue object
 *   limit           : size of a queue (max number of stored messages)
 *
 ******************************************************************************/

#define             OS_MSG( msg, limit )                                \
                       unsigned msg##__buf[limit];                       \
                       msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                       msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : static_MSG
 *
 * Description       : define and initialize a static message queue object
 *
 * Parameters
 *   msg             : name of a pointer to message queue object
 *   limit           : size of a queue (max number of stored messages)
 *
 ******************************************************************************/

#define         static_MSG( msg, limit )                                \
                static unsigned msg##__buf[limit];                       \
                static msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                static msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : MSG_INIT
 *
 * Description       : create and initialize a message queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 * Return            : message queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_INIT( limit ) \
                      _MSG_INIT( limit, _MSG_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : MSG_CREATE
 * Alias             : MSG_NEW
 *
 * Description       : create and initialize a message queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 * Return            : pointer to message queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_CREATE( limit ) \
             & (msg_t) MSG_INIT  ( limit )
#define                MSG_NEW \
                       MSG_CREATE
#endif

/******************************************************************************
 *
 * Name              : msg_init
 *
 * Description       : initialize a message queue object
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   limit           : size of a queue (max number of stored messages)
 *   data            : message queue data buffer
 *
 * Return            : none
 *
 ******************************************************************************/

void msg_init( msg_t *msg, unsigned limit, unsigned *data );

/******************************************************************************
 *
 * Name              : msg_wait
 *
 * Description       : try to transfer message data from the message queue object,
 *                     wait indefinitely while the message queue object is empty
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : pointer to store message data
 *
 * Return            : none
 *
 ******************************************************************************/

void msg_wait( msg_t *msg, unsigned *data );

/******************************************************************************
 *
 * Name              : msg_take
 *
 * Description       : try to transfer message data from the message queue object,
 *                     don't wait if the message queue object is empty
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : pointer to store message data
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered from the message queue object
 *   E_FAILURE       : message queue object is empty
 *
 ******************************************************************************/

unsigned msg_take( msg_t *msg, unsigned *data );

/******************************************************************************
 *
 * Name              : msg_send
 *
 * Description       : try to transfer message data to the message queue object,
 *                     wait indefinitely while the message queue object is full
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : message data
 *
 * Return            : none
 *
 ******************************************************************************/

void msg_send( msg_t *msg, unsigned data );

/******************************************************************************
 *
 * Name              : msg_give
 *
 * Description       : try to transfer message data to the message queue object,
 *                     don't wait if the message queue object is full
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : message data
 *
 * Return
 *   E_SUCCESS       : message data was successfully transfered to the message queue object
 *   E_FAILURE       : message queue object is full
 *
 ******************************************************************************/

unsigned msg_give( msg_t *msg, unsigned data );

/******************************************************************************
 *
 * Name              : msg_push
 *
 * Description       : transfer message data to the message queue object,
 *                     remove the oldest message data if the message queue object is full
 *
 * Parameters
 *   msg             : pointer to message queue object
 *   data            : message data
 *
 * Return            : none
 *
 ******************************************************************************/

void msg_push( msg_t *msg, unsigned data );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseMessageQueue
 *
 * Description       : create and initialize a message queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored messages)
 *   data            : message queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseMessageQueue : public __msg
{
	explicit
	baseMessageQueue( const unsigned _limit, unsigned * const _data ): __msg _MSG_INIT(_limit, _data) {}

	void     wait( unsigned*_data ) {        msg_wait(this, _data); }
	unsigned take( unsigned*_data ) { return msg_take(this, _data); }
	void     send( unsigned _data ) {        msg_send(this, _data); }
	unsigned give( unsigned _data ) { return msg_give(this, _data); }
	void     push( unsigned _data ) {        msg_push(this, _data); }
};

/******************************************************************************
 *
 * Class             : MessageQueue
 *
 * Description       : create and initialize a message queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored messages)
 *
 ******************************************************************************/

template<unsigned _limit>
struct MessageQueueT : public baseMessageQueue
{
	explicit
	MessageQueueT( void ): baseMessageQueue(_limit, data_) {}

	private:
	unsigned data_[_limit];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_MSG_H
