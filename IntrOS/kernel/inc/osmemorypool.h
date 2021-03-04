/******************************************************************************

    @file    IntrOS: osmemorypool.h
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

#ifndef __INTROS_MEM_H
#define __INTROS_MEM_H

#include "oskernel.h"
#include "oslist.h"

/* -------------------------------------------------------------------------- */

#define MEM_SIZE( size ) \
    ALIGNED_SIZE( size, sizeof(que_t) )

/******************************************************************************
 *
 * Name              : memory pool
 *
 ******************************************************************************/

typedef struct __mem mem_t, * const mem_id;

struct __mem
{
	lst_t    lst;   // memory pool list

	unsigned limit; // size of a memory pool (max number of objects)
	unsigned size;  // size of memory object (in sizeof(que_t) units)
	que_t  * data;  // pointer to memory pool buffer
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _MEM_INIT
 *
 * Description       : create and initialize a memory pool object
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in sizeof(que_t) units)
 *   data            : memory pool data buffer
 *
 * Return            : memory pool object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MEM_INIT( _limit, _size, _data ) { _LST_INIT(), _limit, _size, _data }

/******************************************************************************
 *
 * Name              : _MEM_DATA
 *
 * Description       : create a memory pool data buffer
 *
 * Parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in sizeof(que_t) units)
 *
 * Return            : memory pool data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _MEM_DATA( _limit, _size ) (que_t[_limit * (1 + _size)]){ { NULL } }
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

#define             OS_MEM( mem, limit, size )                                          \
                       que_t mem##__buf[limit * (1 + MEM_SIZE(size))];                   \
                       mem_t mem##__mem = _MEM_INIT( limit, MEM_SIZE(size), mem##__buf ); \
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

#define         static_MEM( mem, limit, size )                                          \
                static que_t mem##__buf[limit * (1 + MEM_SIZE(size))];                   \
                static mem_t mem##__mem = _MEM_INIT( limit, MEM_SIZE(size), mem##__buf ); \
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
                      _MEM_INIT( limit, MEM_SIZE(size), _MEM_DATA(limit, MEM_SIZE(size)) )
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
           (mem_t[]) { MEM_INIT  ( limit, size ) }
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
 *   size            : size of memory object (in bytes)
 *   data            : memory pool data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 ******************************************************************************/

void mem_init( mem_t *mem, size_t size, que_t *data, size_t bufsize );

/******************************************************************************
 *
 * Name              : mem_take
 * Alias             : mem_tryWait
 *
 * Description       : try to get memory object from the memory pool object,
 *                     don't wait if the memory pool object is empty
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *
 * Return            : pointer to the memory object
 *   NULL            : memory pool object is empty
 *
 ******************************************************************************/

__STATIC_INLINE
void *mem_take( mem_t *mem ) { return lst_take(&mem->lst); }

__STATIC_INLINE
void *mem_tryWait( mem_t *mem ) { return mem_take(mem); }

/******************************************************************************
 *
 * Name              : mem_wait
 *
 * Description       : try to get memory object from the memory pool object,
 *                     wait indefinitely while the memory pool object is empty
 *
 * Parameters
 *   mem             : pointer to memory pool object
 *
 * Return            : pointer to the memory object
 *
 ******************************************************************************/

__STATIC_INLINE
void *mem_wait( mem_t *mem ) { return lst_wait(&mem->lst); }

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

__STATIC_INLINE
void mem_give( mem_t *mem, void *data ) { lst_give(&mem->lst, data); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : MemoryPoolT<>
 *
 * Description       : create and initialize a memory pool object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of objects)
 *   size            : size of memory object (in bytes)
 *
 ******************************************************************************/

template<unsigned limit_, size_t size_>
struct MemoryPoolT : public __mem
{
	MemoryPoolT( void ): __mem _MEM_INIT(limit_, MEM_SIZE(size_), data_) { mem_bind(this); }

	MemoryPoolT( MemoryPoolT&& ) = default;
	MemoryPoolT( const MemoryPoolT& ) = delete;
	MemoryPoolT& operator=( MemoryPoolT&& ) = delete;
	MemoryPoolT& operator=( const MemoryPoolT& ) = delete;

	void *take   ( void )        { return mem_take   (this); }
	void *tryWait( void )        { return mem_tryWait(this); }
	void *wait   ( void )        { return mem_wait   (this); }
	void  give   ( void *_data ) {        mem_give   (this, _data); }

	private:
	que_t data_[limit_ * (1 + MEM_SIZE(size_))];
};

/******************************************************************************
 *
 * Class             : MemoryPoolTT<>
 *
 * Description       : create and initialize a memory pool object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of objects)
 *   C               : class of a memory object
 *
 ******************************************************************************/

template<unsigned limit_, class C>
struct MemoryPoolTT : public MemoryPoolT<limit_, sizeof(C)>
{
	MemoryPoolTT( void ): MemoryPoolT<limit_, sizeof(C)>() {}

	C *take   ( void ) { return reinterpret_cast<C *>(mem_take   (this)); }
	C *tryWait( void ) { return reinterpret_cast<C *>(mem_tryWait(this)); }
	C *wait   ( void ) { return reinterpret_cast<C *>(mem_wait   (this)); }
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_MEM_H
