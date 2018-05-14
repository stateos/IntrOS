/******************************************************************************

    @file    IntrOS: osmessagebuffer.h
    @author  Rajmund Szymanski
    @date    14.05.2018
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
 * Name              : message buffer
 *
 ******************************************************************************/

typedef struct __msg msg_t, * const msg_id;

struct __msg
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from buffer
	unsigned next;  // next element to write into buffer
	char   * data;  // buffer data

	unsigned size;  // size of the first message in the buffer
};

/******************************************************************************
 *
 * Name              : _MSG_INIT
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : message buffer data
 *
 * Return            : message buffer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MSG_INIT( _limit, _data ) { 0, _limit, 0, 0, _data, 0 }

/******************************************************************************
 *
 * Name              : _MSG_DATA
 *
 * Description       : create a message buffer data
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : message buffer data
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _MSG_DATA( _limit ) (char[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_MSG
 *
 * Description       : define and initialize a message buffer object
 *
 * Parameters
 *   msg             : name of a pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

#define             OS_MSG( msg, limit )                                \
                       char msg##__buf[limit];                           \
                       msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                       msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : static_MSG
 *
 * Description       : define and initialize a static message buffer object
 *
 * Parameters
 *   msg             : name of a pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

#define         static_MSG( msg, limit )                                \
                static char msg##__buf[limit];                           \
                static msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                static msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : MSG_INIT
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : message buffer object
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
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : pointer to message buffer object
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
 * Description       : initialize a message buffer object
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : message buffer data
 *
 * Return            : none
 *
 ******************************************************************************/

void msg_init( msg_t *msg, unsigned limit, void *data );

/******************************************************************************
 *
 * Name              : msg_wait
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait indefinitely while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes read from the message buffer
 *
 ******************************************************************************/

unsigned msg_wait( msg_t *msg, void *data, unsigned size );

/******************************************************************************
 *
 * Name              : msg_take
 *
 * Description       : try to transfer data from the message buffer object,
 *                     don't wait if the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes read from the message buffer
 *
 ******************************************************************************/

unsigned msg_take( msg_t *msg, void *data, unsigned size );

/******************************************************************************
 *
 * Name              : msg_send
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait indefinitely while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return            : number of bytes written to the message buffer
 *
 ******************************************************************************/

unsigned msg_send( msg_t *msg, const void *data, unsigned size );

/******************************************************************************
 *
 * Name              : msg_give
 *
 * Description       : try to transfer data to the message buffer object,
 *                     don't wait if the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return            : number of bytes written to the message buffer
 *
 ******************************************************************************/

unsigned msg_give( msg_t *msg, const void *data, unsigned size );

/******************************************************************************
 *
 * Name              : msg_count
 *
 * Description       : return the size of the first message in the buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : amount of data contained in the message buffer
 *
 ******************************************************************************/

unsigned msg_count( msg_t *msg );

/******************************************************************************
 *
 * Name              : msg_space
 *
 * Description       : return the amount of free space in the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : amount of free space in the message buffer
 *
 ******************************************************************************/

unsigned msg_space( msg_t *msg );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseMessageBuffer
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : message buffer data
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseMessageBuffer : public __msg
{
	explicit
	baseMessageBuffer( const unsigned _limit, char * const _data ): __msg _MSG_INIT(_limit, _data) {}

	unsigned wait (       void *_data, unsigned _size ) { return msg_wait (this, _data, _size); }
	unsigned take (       void *_data, unsigned _size ) { return msg_take (this, _data, _size); }
	unsigned send ( const void *_data, unsigned _size ) { return msg_send (this, _data, _size); }
	unsigned give ( const void *_data, unsigned _size ) { return msg_give (this, _data, _size); }
	unsigned count( void )                              { return msg_count(this);               }
	unsigned space( void )                              { return msg_space(this);               }
};

/******************************************************************************
 *
 * Class             : MessageBuffer
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

template<unsigned _limit>
struct MessageBufferT : public baseMessageBuffer
{
	explicit
	MessageBufferT( void ): baseMessageBuffer(sizeof(data_), data_) {}

	private:
	char data_[_limit];
};

/******************************************************************************
 *
 * Class             : MessageBuffer
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   T               : class of an object
 *
 ******************************************************************************/

template<unsigned _limit, class T>
struct MessageBufferTT : public baseMessageBuffer
{
	explicit
	MessageBufferTT( void ): baseMessageBuffer(sizeof(data_), data_) {}

	private:
	char data_[_limit*(sizeof(T)+sizeof(unsigned))-sizeof(unsigned)];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_MSG_H
