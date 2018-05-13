/******************************************************************************

    @file    IntrOS: osmemorypool.h
    @author  Rajmund Szymanski
    @date    13.05.2018
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

#ifndef __INTROS_MEM_H
#define __INTROS_MEM_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : memory pool
 *
 ******************************************************************************/

typedef struct __mem mem_t, * const mem_id;

struct __mem
{
	que_t  * next;  // inherited from list
	unsigned limit; // size of a memory pool (max number of objects)
	unsigned size;  // size of memory object (in words)
	void   * data;  // pointer to memory pool buffer
};

/* -------------------------------------------------------------------------- */

#define MSIZE( size ) \
 ALIGNED_SIZE( size, que_t )

/******************************************************************************
 *
 * Name              : _MEM_INIT
 *
 * Description       : create and initialize a memory pool object
 *
 * Parameters
 *   size            : size of memory object (in bytes)
 *   data            : memory pool data buffer
 *
 * Return            : memory pool object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MEM_INIT( _limit, _size, _data ) { 0, _limit, MSIZE(_size), _data }

/******************************************************************************
 *
 * Name              : _MEM_DATA
 *
 * Description       : create a memory pool data buffer
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 * Return            : memory pool data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _MEM_DATA( _limit, _size ) (void *[_limit * (1 + MSIZE(_size))]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_MEM
 *
 * Description       : define and initialize a memory pool object
 *
 * Parameters
 *   mem             : name of a pointer to memory pool object
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 ******************************************************************************/

#define             OS_MEM( mem, limit, size )                                \
                       void*mem##__buf[limit*(1+MSIZE(size))];                 \
                       mem_t mem##__mem = _MEM_INIT( limit, size, mem##__buf ); \
                       mem_id mem = & mem##__mem

/******************************************************************************
 *
 * Name              : static_MEM
 *
 * Description       : define and initialize a static memory pool object
 *
 * Parameters
 *   mem             : name of a pointer to memory pool object
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 ******************************************************************************/

#define         static_MEM( mem, limit, size )                                \
                static void*mem##__buf[limit*(1+MSIZE(size))];                 \
                static mem_t mem##__mem = _MEM_INIT( limit, size, mem##__buf ); \
                static mem_id mem = & mem##__mem

/******************************************************************************
 *
 * Name              : MEM_INIT
 *
 * Description       : create and initialize a memory pool object
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 * Return            : memory pool object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MEM_INIT( limit, size ) \
                      _MEM_INIT( limit, size, _MEM_DATA( limit, size ) )
#endif

/******************************************************************************
 *
 * Name              : MEM_CREATE
 * Alias             : MEM_NEW
 *
 * Description       : create and initialize a memory pool object
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 * Return            : pointer to memory pool object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MEM_CREATE( limit, size ) \
             & (mem_t) MEM_INIT  ( limit, size )
#define                MEM_NEW \
                       MEM_CREATE
#endif

/******************************************************************************
 *
 * Name              : mem_bind
 *
 * Description       : initialize data buffer of a memory pool object
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *
 * Return            : none
 *
 ******************************************************************************/

void mem_bind( mem_t *mem );

/******************************************************************************
 *
 * Name              : mem_init
 *
 * Description       : initialize a memory pool object
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *   data            : memory pool data buffer
 *
 * Return            : none
 *
 ******************************************************************************/

void mem_init( mem_t *mem, unsigned limit, unsigned size, void *data );

/******************************************************************************
 *
 * Name              : mem_wait
 *
 * Description       : try to get memory object from the memory pool object,
 *                     wait indefinitely while the memory pool object is empty
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   data            : pointer to store the pointer to the memory object
 *
 * Return            : none
 *
 ******************************************************************************/

void mem_wait( mem_t *mem, void **data );

/******************************************************************************
 *
 * Name              : mem_take
 *
 * Description       : try to get memory object from the memory pool object,
 *                     don't wait if the memory pool object is empty
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   data            : pointer to store the pointer to the memory object
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer
 *   E_FAILURE       : memory pool object is empty
 *
 ******************************************************************************/

unsigned mem_take( mem_t *mem, void **data );

/******************************************************************************
 *
 * Name              : mem_give
 *
 * Description       : transfer memory object to the memory pool object,
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *   data            : pointer to memory object
 *
 * Return            : none
 *
 ******************************************************************************/

void mem_give( mem_t *mem, const void *data );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseMemoryPool
 *
 * Description       : create and initialize a memory pool object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *   data            : memory pool data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseMemoryPool : public __mem
{
	explicit
	baseMemoryPool( const unsigned _limit, const unsigned _size, void * const _data ): __mem _MEM_INIT(_limit, _size, _data) { mem_bind(this); }

	void     wait(       void **_data ) {        mem_wait(this, _data); }
	unsigned take(       void **_data ) { return mem_take(this, _data); }
	void     give( const void  *_data ) {        mem_give(this, _data); }
};

/******************************************************************************
 *
 * Class             : MemoryPool
 *
 * Description       : create and initialize a memory pool object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 ******************************************************************************/

template<unsigned _limit, unsigned _size>
struct MemoryPoolT : public baseMemoryPool
{
	explicit
	MemoryPoolT( void ): baseMemoryPool(_limit, _size, reinterpret_cast<void *>(data_)) {}

	private:
	void *data_[_limit * (1 + MSIZE(_size))];
};

/******************************************************************************
 *
 * Class             : MemoryPool
 *
 * Description       : create and initialize a memory pool object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of objects)
 *   T               : class of a memory object
 *
 ******************************************************************************/

template<unsigned _limit, class T>
struct MemoryPoolTT : public MemoryPoolT<_limit, sizeof(T)>
{
	explicit
	MemoryPoolTT( void ): MemoryPoolT<_limit, sizeof(T)>() {}

	void     wait( T **_data ) {        mem_wait(this, reinterpret_cast<void **>(_data)); }
	unsigned take( T **_data ) { return mem_take(this, reinterpret_cast<void **>(_data)); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_MEM_H
