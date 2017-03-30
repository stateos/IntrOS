/******************************************************************************

    @file    IntrOS: os_mem.h
    @author  Rajmund Szymanski
    @date    30.03.2017
    @brief   This file contains definitions for IntrOS.

 ******************************************************************************

    IntrOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of IntrOS distribution.

    IntrOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    IntrOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef __INTROS_MEM_H
#define __INTROS_MEM_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : memory pool                                                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

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
 (((unsigned)( size )+(sizeof(void*)-1))/sizeof(void*))

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _MEM_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   size            : size of memory object (in bytes)                                                               *
 *   data            : memory pool data buffer                                                                        *
 *                                                                                                                    *
 * Return            : memory pool object                                                                             *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _MEM_INIT( _limit, _size, _data ) { 0, _limit, MSIZE(_size), _data }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _MEM_DATA                                                                                      *
 *                                                                                                                    *
 * Description       : create a memory pool data buffer                                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : memory pool data buffer                                                                        *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _MEM_DATA( _limit, _size ) (void *[_limit * (1 + MSIZE(_size))]){ 0 }
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_MEM                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : name of a pointer to memory pool object                                                        *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_MEM( mem, limit, size )                                \
                       void*mem##__buf[limit*(1+MSIZE(size))];                 \
                       mem_t mem##__mem = _MEM_INIT( limit, size, mem##__buf ); \
                       mem_id mem = & mem##__mem

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_MEM                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static memory pool object                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : name of a pointer to memory pool object                                                        *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_MEM( mem, limit, size )                                \
                static void*mem##__buf[limit*(1+MSIZE(size))];                 \
                static mem_t mem##__mem = _MEM_INIT( limit, size, mem##__buf ); \
                static mem_id mem = & mem##__mem

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MEM_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : memory pool object                                                                             *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MEM_INIT( limit, size ) \
                      _MEM_INIT( limit, size, _MEM_DATA( limit, size ) )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MEM_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : pointer to memory pool object                                                                  *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MEM_CREATE( limit, size ) \
             & (mem_t) MEM_INIT( limit, size )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_bind                                                                                       *
 *                                                                                                                    *
 * Description       : initialize data buffer of a memory pool object                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mem_bind( mem_t *mem );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to get memory object from the memory pool object,                                          *
 *                     wait indefinitly while the memory pool object is empty                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mem_wait( mem_t *mem, void **data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to get memory object from the memory pool object,                                          *
 *                     don't wait if the memory pool object is empty                                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_FAILURE       : memory pool object is empty                                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned mem_take( mem_t *mem, void **data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mem_give                                                                                       *
 *                                                                                                                    *
 * Description       : transfer memory object to the memory pool object,                                              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mem             : pointer to memory pool object                                                                  *
 *   data            : pointer to memory object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mem_give( mem_t *mem, void *data );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : MemoryPool                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   size            : size of memory object (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _limit, unsigned _size>
struct MemoryPoolT : public __mem
{
	explicit
	MemoryPoolT( void ): __mem _MEM_INIT(_limit, _size, _data) { mem_bind(this); }

	void     wait( void **_data ) {        mem_wait(this, (void**)_data); }
	unsigned take( void **_data ) { return mem_take(this, (void**)_data); }
	void     give( void  *_data ) {        mem_give(this,         _data); }

	private:
	void *_data[_limit * (1 + MSIZE(_size))];
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : MemoryPool                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a memory pool object                                                      *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a buffer (max number of objects)                                                       *
 *   T               : class of a memory object                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _limit, class T>
struct MemoryPoolTT : public MemoryPoolT<_limit, sizeof(T)>
{
	void     wait( T **_data ) {        mem_wait(this, (void**)_data); }
	unsigned take( T **_data ) { return mem_take(this, (void**)_data); }
	void     give( T  *_data ) {        mem_give(this,         _data); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_MEM_H
