/******************************************************************************

    @file    IntrOS: os_lst.h
    @author  Rajmund Szymanski
    @date    13.03.2018
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

#ifndef __INTROS_LST_H
#define __INTROS_LST_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : list
 *
 ******************************************************************************/

typedef struct __lst lst_t, * const lst_id;

struct __lst
{
	que_t  * next;  // next memory object in the queue, previously created in the memory pool
};

/******************************************************************************
 *
 * Name              : _LST_INIT
 *
 * Description       : create and initialize a list object
 *
 * Parameters        : none
 *
 * Return            : list object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _LST_INIT() { 0 }

/******************************************************************************
 *
 * Name              : OS_LST
 *
 * Description       : define and initialize a list object
 *
 * Parameters
 *   lst             : name of a pointer to list object
 *
 ******************************************************************************/

#define             OS_LST( lst )                     \
                       lst_t lst##__lst = _LST_INIT(); \
                       lst_id lst = & lst##__lst

/******************************************************************************
 *
 * Name              : static_LST
 *
 * Description       : define and initialize a static list object
 *
 * Parameters
 *   lst             : name of a pointer to list object
 *
 ******************************************************************************/

#define         static_LST( lst )                     \
                static lst_t lst##__lst = _LST_INIT(); \
                static lst_id lst = & lst##__lst

/******************************************************************************
 *
 * Name              : LST_INIT
 *
 * Description       : create and initialize a list object
 *
 * Parameters        : none
 *
 * Return            : list object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                LST_INIT() \
                      _LST_INIT()
#endif

/******************************************************************************
 *
 * Name              : LST_CREATE
 * Alias             : LST_NEW
 *
 * Description       : create and initialize a list object
 *
 * Parameters        : none
 *
 * Return            : pointer to list object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                LST_CREATE() \
             & (lst_t) LST_INIT  ()
#define                LST_NEW \
                       LST_CREATE
#endif

/******************************************************************************
 *
 * Name              : lst_init
 *
 * Description       : initialize a list object
 *
 * Parameters
 *   lst             : pointer to list object
 *
 * Return            : none
 *
 ******************************************************************************/

void lst_init( lst_t *lst );

/******************************************************************************
 *
 * Name              : lst_wait
 *
 * Description       : try to get memory object from the list object,
 *                     wait indefinitely while the list object is empty
 *
 * Parameters
 *   lst             : pointer to list object
 *   data            : pointer to store the pointer to the memory object
 *
 * Return            : none
 *
 ******************************************************************************/

void lst_wait( lst_t *lst, void **data );

/******************************************************************************
 *
 * Name              : lst_take
 *
 * Description       : try to get memory object from the list object,
 *                     don't wait if the list object is empty
 *
 * Parameters
 *   lst             : pointer to list object
 *   data            : pointer to store the pointer to the memory object
 *
 * Return
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer
 *   E_FAILURE       : list object is empty
 *
 ******************************************************************************/

unsigned lst_take( lst_t *lst, void **data );

/******************************************************************************
 *
 * Name              : lst_give
 *
 * Description       : transfer memory object to the list object,
 *
 * Parameters
 *   lst             : pointer to list object
 *   data            : pointer to memory object
 *
 * Return            : none
 *
 ******************************************************************************/

void lst_give( lst_t *lst, void *data );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : List
 *
 * Description       : create and initialize a list object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct List : public __lst
{
	explicit
	List( void ): __lst _LST_INIT() {}

	void     wait( void **_data ) {        lst_wait(this, _data); }
	unsigned take( void **_data ) { return lst_take(this, _data); }
	void     give( void  *_data ) {        lst_give(this, _data); }
};

/******************************************************************************
 *
 * Class             : List
 *
 * Description       : create and initialize a list object
 *
 * Constructor parameters
 *   T               : class of a list object
 *
 ******************************************************************************/

template<class T>
struct ListT : public List
{
	explicit
	ListT( void ): List() {}

	void     wait( T **_data ) {        lst_wait(this, reinterpret_cast<void **>(_data)); }
	unsigned take( T **_data ) { return lst_take(this, reinterpret_cast<void **>(_data)); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_LST_H
