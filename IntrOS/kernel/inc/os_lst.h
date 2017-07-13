/******************************************************************************

    @file    IntrOS: os_lst.h
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

#ifndef __INTROS_LST_H
#define __INTROS_LST_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : list                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __lst lst_t, * const lst_id;

struct __lst
{
	que_t  * next;  // next memory object in the queue, previously created in the memory pool
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _LST_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a list object                                                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : list object                                                                                    *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _LST_INIT() { 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_LST                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a list object                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : name of a pointer to list object                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_LST( lst )                     \
                       lst_t lst##__lst = _LST_INIT(); \
                       lst_id lst = & lst##__lst

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_LST                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static list object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : name of a pointer to list object                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_LST( lst )                     \
                static lst_t lst##__lst = _LST_INIT(); \
                static lst_id lst = & lst##__lst

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : LST_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a list object                                                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : list object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                LST_INIT() \
                      _LST_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : LST_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a list object                                                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to list object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                LST_CREATE() \
             & (lst_t) LST_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to get memory object from the list object,                                                 *
 *                     wait indefinitly while the list object is empty                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void lst_wait( lst_t *lst, void **data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to get memory object from the list object,                                                 *
 *                     don't wait if the list object is empty                                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to store the pointer to the memory object                                              *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : pointer to memory object was successfully transfered to the data pointer                       *
 *   E_FAILURE       : list object is empty                                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned lst_take( lst_t *lst, void **data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : lst_give                                                                                       *
 *                                                                                                                    *
 * Description       : transfer memory object to the list object,                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   lst             : pointer to list object                                                                         *
 *   data            : pointer to memory object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void lst_give( lst_t *lst, void *data );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : List                                                                                           *
 *                                                                                                                    *
 * Description       : create and initilize a list object                                                             *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct List : public __lst
{
	explicit
	List( void ): __lst _LST_INIT() {}

	void     wait( void **_data ) {        lst_wait(this, _data); }
	unsigned take( void **_data ) { return lst_take(this, _data); }
	void     give( void  *_data ) {        lst_give(this, _data); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_LST_H
