/******************************************************************************

    @file    IntrOS: osmessagebuffer.h
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

#ifndef __INTROS_MSG_H
#define __INTROS_MSG_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : message buffer
 *
 ******************************************************************************/

typedef struct __msg msg_t, * const msg_id;

struct __msg
{
	size_t   count; // inherited from stream buffer
	size_t   limit; // inherited from stream buffer

	size_t   head;  // inherited from stream buffer
	size_t   tail;  // inherited from stream buffer
	char *   data;  // inherited from stream buffer
};

#ifdef __cplusplus
extern "C" {
#endif

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

#define               _MSG_INIT( _limit, _data ) { 0, _limit, 0, 0, _data }

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
 * Name              : _VA_MSG
 *
 * Description       : calculate buffer size from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_MSG( _limit, _size ) ( (_size + 0) ? ((_limit) * (sizeof(size_t) + (_size + 0))) : (_limit) )

/******************************************************************************
 *
 * Name              : OS_MSG
 *
 * Description       : define and initialize a message buffer object
 *
 * Parameters
 *   msg             : name of a pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes)
 *
 ******************************************************************************/

#define             OS_MSG( msg, limit, ... )                                                 \
                       char msg##__buf[_VA_MSG(limit, __VA_ARGS__)];                           \
                       msg_t msg##__msg = _MSG_INIT( _VA_MSG(limit, __VA_ARGS__), msg##__buf ); \
                       msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : static_MSG
 *
 * Description       : define and initialize a static message buffer object
 *
 * Parameters
 *   msg             : name of a pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes)
 *
 ******************************************************************************/

#define         static_MSG( msg, limit, ... )                                                 \
                static char msg##__buf[_VA_MSG(limit, __VA_ARGS__)];                           \
                static msg_t msg##__msg = _MSG_INIT( _VA_MSG(limit, __VA_ARGS__), msg##__buf ); \
                static msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : MSG_INIT
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes)
 *
 * Return            : message buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_INIT( limit, ... ) \
                      _MSG_INIT( _VA_MSG(limit, __VA_ARGS__), _MSG_DATA( _VA_MSG(limit, __VA_ARGS__) ) )
#endif

/******************************************************************************
 *
 * Name              : MSG_CREATE
 * Alias             : MSG_NEW
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes)
 *
 * Return            : pointer to message buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_CREATE( limit, ... ) \
           (msg_t[]) { MSG_INIT  ( _VA_MSG(limit, __VA_ARGS__) ) }
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
 *   data            : message buffer data
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 ******************************************************************************/

void msg_init( msg_t *msg, void *data, size_t bufsize );

/******************************************************************************
 *
 * Name              : msg_take
 * Alias             : msg_tryWait
 *
 * Description       : try to transfer data from the message buffer object,
 *                     don't wait if the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return            : number of read bytes
 *   0               : message buffer object is empty or not enough space in the buffer
 *
 ******************************************************************************/

size_t msg_take( msg_t *msg, void *data, size_t size );

__STATIC_INLINE
size_t msg_tryWait( msg_t *msg, void *data, size_t size ) { return msg_take(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_wait
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait indefinitely while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return            : number of read bytes
 *
 ******************************************************************************/

size_t msg_wait( msg_t *msg, void *data, size_t size );

/******************************************************************************
 *
 * Name              : msg_give
 *
 * Description       : try to transfer data to the message buffer object,
 *                     don't wait if the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   SUCCESS         : message data was successfully transferred to the message buffer object
 *   FAILURE         : not enough space in the message buffer
 *
 ******************************************************************************/

unsigned msg_give( msg_t *msg, const void *data, size_t size );

/******************************************************************************
 *
 * Name              : msg_send
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait indefinitely while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   SUCCESS         : message data was successfully transferred to the message buffer object
 *   FAILURE         : size of the message data is out of the limit
 *
 ******************************************************************************/

unsigned msg_send( msg_t *msg, const void *data, size_t size );

/******************************************************************************
 *
 * Name              : msg_push
 *
 * Description       : try to transfer data to the message buffer object,
 *                     remove the oldest data if the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to the buffer
 *   size            : size of the buffer
 *
 * Return
 *   SUCCESS         : message data was successfully transferred to the message buffer object
 *   FAILURE         : size of the message data is out of the limit
 *
 ******************************************************************************/

unsigned msg_push( msg_t *msg, const void *data, size_t size );

/******************************************************************************
 *
 * Name              : msg_count
 *
 * Description       : return the amount of data contained in the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : amount of data contained in the message buffer
 *
 ******************************************************************************/

size_t msg_count( msg_t *msg );

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

size_t msg_space( msg_t *msg );

/******************************************************************************
 *
 * Name              : msg_limit
 *
 * Description       : return the size of the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : size of the message buffer
 *
 ******************************************************************************/

size_t msg_limit( msg_t *msg );

/******************************************************************************
 *
 * Name              : msg_size
 *
 * Description       : return the size of first message in the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : size of first message in the message buffer
 *
 ******************************************************************************/

size_t msg_size( msg_t *msg );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : MessageBufferT<>
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

template<size_t limit_>
struct MessageBufferT : public __msg
{
	constexpr
	MessageBufferT( void ): __msg _MSG_INIT(limit_, data_) {}

	MessageBufferT( MessageBufferT&& ) = default;
	MessageBufferT( const MessageBufferT& ) = delete;
	MessageBufferT& operator=( MessageBufferT&& ) = delete;
	MessageBufferT& operator=( const MessageBufferT& ) = delete;

	size_t   take   (       void *_data, size_t _size ) { return msg_take   (this, _data, _size); }
	size_t   tryWait(       void *_data, size_t _size ) { return msg_tryWait(this, _data, _size); }
	size_t   wait   (       void *_data, size_t _size ) { return msg_wait   (this, _data, _size); }
	unsigned give   ( const void *_data, size_t _size ) { return msg_give   (this, _data, _size); }
	unsigned send   ( const void *_data, size_t _size ) { return msg_send   (this, _data, _size); }
	unsigned push   ( const void *_data, size_t _size ) { return msg_push   (this, _data, _size); }
	size_t   count  ( void )                            { return msg_count  (this); }
	size_t   space  ( void )                            { return msg_space  (this); }
	size_t   limit  ( void )                            { return msg_limit  (this); }
	size_t   size   ( void )                            { return msg_size   (this); }

	private:
	char data_[limit_];
};

/******************************************************************************
 *
 * Class             : MessageBufferTT<>
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   C               : class of an object
 *
 ******************************************************************************/

template<unsigned limit_, class C>
struct MessageBufferTT : public MessageBufferT<limit_*(sizeof(size_t)+sizeof(C))>
{
	constexpr
	MessageBufferTT( void ): MessageBufferT<limit_*(sizeof(size_t)+sizeof(C))>() {}

	unsigned take   (       C *_data ) { return msg_take   (this, _data, sizeof(C)) == 0 ? FAILURE : SUCCESS; }
	unsigned tryWait(       C *_data ) { return msg_tryWait(this, _data, sizeof(C)) == 0 ? FAILURE : SUCCESS; }
	void     wait   (       C *_data ) {        msg_wait   (this, _data, sizeof(C)); }
	unsigned give   ( const C *_data ) { return msg_give   (this, _data, sizeof(C)); }
	unsigned send   ( const C *_data ) { return msg_send   (this, _data, sizeof(C)); }
	unsigned push   ( const C *_data ) { return msg_push   (this, _data, sizeof(C)); }
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_MSG_H
