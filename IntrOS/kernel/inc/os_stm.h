/******************************************************************************

    @file    IntrOS: os_stm.h
    @author  Rajmund Szymanski
    @date    16.04.2018
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

#ifndef __INTROS_STM_H
#define __INTROS_STM_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

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

	unsigned first; // first element to read from buffer
	unsigned next;  // next element to write into buffer
	char   * data;  // buffer data

	bool     rdr;   // stream buffer reader
	bool     wtr;   // stream buffer writer
};

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

#define               _STM_INIT( _limit, _data ) { 0, _limit, 0, 0, _data, false, false }

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
 * Name              : OS_STM
 *
 * Description       : define and initialize a stream buffer object
 *
 * Parameters
 *   stm             : name of a pointer to stream buffer object
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

#define             OS_STM( stm, limit )                                \
                       char stm##__buf[limit];                           \
                       stm_t stm##__stm = _STM_INIT( limit, stm##__buf ); \
                       stm_id stm = & stm##__stm

/******************************************************************************
 *
 * Name              : static_STM
 *
 * Description       : define and initialize a static stream buffer object
 *
 * Parameters
 *   stm             : name of a pointer to stream buffer object
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

#define         static_STM( stm, limit )                                \
                static char stm##__buf[limit];                           \
                static stm_t stm##__stm = _STM_INIT( limit, stm##__buf ); \
                static stm_id stm = & stm##__stm

/******************************************************************************
 *
 * Name              : STM_INIT
 *
 * Description       : create and initialize a stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : stream buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                STM_INIT( limit ) \
                      _STM_INIT( limit, _STM_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : STM_CREATE
 * Alias             : STM_NEW
 *
 * Description       : create and initialize a stream buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : pointer to stream buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                STM_CREATE( limit ) \
             & (stm_t) STM_INIT  ( limit )
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
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : stream buffer data
 *
 * Return            : none
 *
 ******************************************************************************/

void stm_init( stm_t *stm, unsigned limit, void *data );

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

__STATIC_INLINE
unsigned stm_count( stm_t *stm ) { return stm->count; }

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

__STATIC_INLINE
unsigned stm_space( stm_t *stm ) { return stm->limit - stm->count; }

/******************************************************************************
 *
 * Name              : stm_wait
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     wait indefinitely while the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return            : none
 *
 ******************************************************************************/

void stm_wait( stm_t *stm, void *data, unsigned size );

/******************************************************************************
 *
 * Name              : stm_take
 *
 * Description       : try to transfer data from the stream buffer object,
 *                     don't wait if the stream buffer object is empty
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return            : number of bytes read
 *
 ******************************************************************************/

unsigned stm_take( stm_t *stm, void *data, unsigned size );

/******************************************************************************
 *
 * Name              : stm_send
 *
 * Description       : try to transfer stream data to the stream buffer object,
 *                     wait indefinitely while the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : none
 *
 ******************************************************************************/

void stm_send( stm_t *stm, const void *data, unsigned size );

/******************************************************************************
 *
 * Name              : stm_give
 *
 * Description       : try to transfer stream data to the stream buffer object,
 *                     don't wait if the stream buffer object is full
 *
 * Parameters
 *   stm             : pointer to stream buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes written
 *
 ******************************************************************************/

unsigned stm_give( stm_t *stm, const void *data, unsigned size );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseStreamBuffer
 *
 * Description       : create and initialize a stream buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : stream buffer data
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseStreamBuffer : public __stm
{
	explicit
	baseStreamBuffer( const unsigned _limit, char * const _data ): __stm _STM_INIT(_limit, _data) {}

	unsigned count( void )                              { return stm_count(this);               }
	unsigned space( void )                              { return stm_space(this);               }
	void     wait (       void *_data, unsigned _size ) {        stm_wait (this, _data, _size); }
	unsigned take (       void *_data, unsigned _size ) { return stm_take (this, _data, _size); }
	void     send ( const void *_data, unsigned _size ) {        stm_send (this, _data, _size); }
	unsigned give ( const void *_data, unsigned _size ) { return stm_give (this, _data, _size); }
};

/******************************************************************************
 *
 * Class             : StreamBuffer
 *
 * Description       : create and initialize a stream buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

template<unsigned _limit>
struct StreamBufferT : public baseStreamBuffer
{
	explicit
	StreamBufferT( void ): baseStreamBuffer(_limit, data_) {}

	private:
	char data_[_limit];
};

/******************************************************************************
 *
 * Class             : StreamBuffer
 *
 * Description       : create and initialize a stream buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   T               : class of an object
 *
 ******************************************************************************/

template<unsigned _limit, class T>
struct StreamBufferTT : public baseStreamBuffer
{
	explicit
	StreamBufferTT( void ): baseStreamBuffer(_limit * sizeof(T), reinterpret_cast<char *>(data_) ) {}

	private:
	T data_[_limit];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_STM_H
