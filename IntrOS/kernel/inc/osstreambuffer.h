/******************************************************************************

    @file    IntrOS: osstreambuffer.h
    @author  Rajmund Szymanski
    @date    14.05.2020
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

#ifndef __INTROS_STM_H
#define __INTROS_STM_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : stream buffer
 *
 ******************************************************************************/

typedef struct __stm stm_t, * const stm_id;

struct __stm
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	char   * data;  // data buffer
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _STM_INIT
 *
 * Description       : create and initialize a stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : stream buffer data
 *
 * Return            : stream buffer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _STM_INIT( _limit, _data ) { 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _STM_DATA
 *
 * Description       : create a stream buffer data
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : stream buffer data
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _STM_DATA( _limit ) (char[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : _VA_STM
 *
 * Description       : calculate buffer size from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_STM( _limit, _size ) ( (_size + 0) ? ((_limit) * (_size + 0)) : (_limit) )

/******************************************************************************
 *
 * Name              : OS_STM
 *
 * Description       : define and initialize a stream buffer object
 *
 * Parameters
 *   stm             : name of a pointer to stream buffer object
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes); default: 1
 *
 ******************************************************************************/

#define             OS_STM( stm, limit, ... )                                                   \
                       struct { stm_t stm; char buf[_VA_STM(limit, __VA_ARGS__)]; } stm##__wrk = \
                       { _STM_INIT( _VA_STM(limit, __VA_ARGS__), stm##__wrk.buf ), { 0 } };       \
                       stm_id stm = & stm##__wrk.stm

/******************************************************************************
 *
 * Name              : static_STM
 *
 * Description       : define and initialize a static stream buffer object
 *
 * Parameters
 *   stm             : name of a pointer to stream buffer object
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes); default: 1
 *
 ******************************************************************************/

#define         static_STM( stm, limit, ... )                                                   \
                static struct { stm_t stm; char buf[_VA_STM(limit, __VA_ARGS__)]; } stm##__wrk = \
                       { _STM_INIT( _VA_STM(limit, __VA_ARGS__), stm##__wrk.buf ), { 0 } };       \
                static stm_id stm = & stm##__wrk.stm

/******************************************************************************
 *
 * Name              : STM_INIT
 *
 * Description       : create and initialize a stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes); default: 1
 *
 * Return            : stream buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                STM_INIT( limit, ... ) \
                      _STM_INIT( _VA_STM(limit, __VA_ARGS__), _STM_DATA( _VA_STM(limit, __VA_ARGS__) ) )
#endif

/******************************************************************************
 *
 * Name              : STM_CREATE
 * Alias             : STM_NEW
 *
 * Description       : create and initialize a stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes / objects)
 *   type            : (optional) size of the object (in bytes); default: 1
 *
 * Return            : pointer to stream buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                STM_CREATE( limit, ... ) \
           (stm_t[]) { STM_INIT  ( _VA_STM(limit, __VA_ARGS__) ) }
#define                STM_NEW \
                       STM_CREATE
#endif

/******************************************************************************
 *
 * Name              : stm_init
 *
 * Description       : initialize a stream buffer object
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : stream buffer data
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 ******************************************************************************/

void stm_init( stm_t *stm, void *data, unsigned bufsize );

/******************************************************************************
 *
 * Name              : stm_take
 * Alias             : stm_tryWait
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     don't wait if the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes read from the stream buffer or
 *   E_FAILURE       : stream buffer object is empty
 *
 ******************************************************************************/

unsigned stm_take( stm_t *stm, void *data, unsigned size );

__STATIC_INLINE
unsigned stm_tryWait( stm_t *stm, void *data, unsigned size ) { return stm_take(stm, data, size); }

/******************************************************************************
 *
 * Name              : stm_wait
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     wait indefinitely while the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes read from the stream buffer
 *
 ******************************************************************************/

unsigned stm_wait( stm_t *stm, void *data, unsigned size );

/******************************************************************************
 *
 * Name              : stm_give
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     don't wait if the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : stream data was successfully transferred to the stream buffer object
 *   E_FAILURE       : not enough space in the stream buffer
 *
 ******************************************************************************/

unsigned stm_give( stm_t *stm, const void *data, unsigned size );

/******************************************************************************
 *
 * Name              : stm_send
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     wait indefinitely while the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : stream data was successfully transferred to the stream buffer object
 *   E_FAILURE       : size of the stream data is out of the limit
 *
 ******************************************************************************/

unsigned stm_send( stm_t *stm, const void *data, unsigned size );

/******************************************************************************
 *
 * Name              : stm_push
 *
 * Description       : try to transfer data to the stream buffer object,
 *                     remove the oldest data if the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 *   E_SUCCESS       : stream data was successfully transferred to the stream buffer object
 *   E_FAILURE       : size of the stream data is out of the limit
 *
 ******************************************************************************/

unsigned stm_push( stm_t *stm, const void *data, unsigned size );

/******************************************************************************
 *
 * Name              : stm_count
 *
 * Description       : return the amount of data contained in the stream buffer
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : amount of data contained in the stream buffer
 *
 ******************************************************************************/

unsigned stm_count( stm_t *stm );

/******************************************************************************
 *
 * Name              : stm_space
 *
 * Description       : return the amount of free space in the stream buffer
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : amount of free space in the stream buffer
 *
 ******************************************************************************/

unsigned stm_space( stm_t *stm );


/******************************************************************************
 *
 * Name              : stm_limit
 *
 * Description       : return the size of the stream buffer
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *
 * Return            : size of the stream buffer
 *
 ******************************************************************************/

unsigned stm_limit( stm_t *stm );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : StreamBufferT<>
 *
 * Description       : create and initialize a stream buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

template<unsigned limit_>
struct StreamBufferT : public __stm
{
	StreamBufferT( void ): __stm _STM_INIT(limit_, data_) {}

	StreamBufferT( StreamBufferT&& ) = default;
	StreamBufferT( const StreamBufferT& ) = delete;
	StreamBufferT& operator=( StreamBufferT&& ) = delete;
	StreamBufferT& operator=( const StreamBufferT& ) = delete;

	unsigned take   (       void *_data, unsigned _size ) { return stm_take   (this, _data, _size); }
	unsigned tryWait(       void *_data, unsigned _size ) { return stm_tryWait(this, _data, _size); }
	unsigned wait   (       void *_data, unsigned _size ) { return stm_wait   (this, _data, _size); }
	unsigned give   ( const void *_data, unsigned _size ) { return stm_give   (this, _data, _size); }
	unsigned send   ( const void *_data, unsigned _size ) { return stm_send   (this, _data, _size); }
	unsigned push   ( const void *_data, unsigned _size ) { return stm_push   (this, _data, _size); }
	unsigned count  ( void )                              { return stm_count  (this);               }
	unsigned space  ( void )                              { return stm_space  (this);               }
	unsigned limit  ( void )                              { return stm_limit  (this);               }

	private:
	char data_[limit_];
};

/******************************************************************************
 *
 * Class             : StreamBufferTT<>
 *
 * Description       : create and initialize a stream buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   C               : class of an object
 *
 ******************************************************************************/

template<unsigned limit_, class C>
struct StreamBufferTT : public StreamBufferT<limit_*sizeof(C)>
{
	StreamBufferTT( void ): StreamBufferT<limit_*sizeof(C)>() {}

	unsigned take   (       C *_data ) { return stm_take   (this, _data, sizeof(C)); }
	unsigned tryWait(       C *_data ) { return stm_tryWait(this, _data, sizeof(C)); }
	unsigned wait   (       C *_data ) { return stm_wait   (this, _data, sizeof(C)); }
	unsigned give   ( const C *_data ) { return stm_give   (this, _data, sizeof(C)); }
	unsigned send   ( const C *_data ) { return stm_send   (this, _data, sizeof(C)); }
	unsigned push   ( const C *_data ) { return stm_push   (this, _data, sizeof(C)); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_STM_H
