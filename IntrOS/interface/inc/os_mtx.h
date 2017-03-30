/******************************************************************************

    @file    IntrOS: os_mtx.h
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

#ifndef __INTROS_MTX_H
#define __INTROS_MTX_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mutex                                                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __mtx mtx_t, * const mtx_id;

struct __mtx
{
	tsk_t  * owner; // owner task
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _MTX_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : mutex object                                                                                   *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _MTX_INIT() { 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_MTX                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : name of a pointer to mutex object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_MTX( mtx )                     \
                       mtx_t mtx##__mtx = _MTX_INIT(); \
                       mtx_id mtx = & mtx##__mtx

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_MTX                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static mutex object                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : name of a pointer to mutex object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_MTX( mtx )                     \
                static mtx_t mtx##__mtx = _MTX_INIT(); \
                static mtx_id mtx = & mtx##__mtx

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MTX_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : mutex object                                                                                   *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MTX_INIT() \
                      _MTX_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MTX_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MTX_CREATE() \
             & (mtx_t) MTX_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to lock the mutex object,                                                                  *
 *                     wait indefinitly if the mutex object can't be locked immediately                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void mtx_wait( mtx_t *mtx );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to lock the mutex object,                                                                  *
 *                     don't wait if the mutex object can't be locked immediately                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_FAILURE       : mutex object can't be locked immediately                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned mtx_take( mtx_t *mtx );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to unlock the mutex object (only owner task can unlock mutex object),                      *
 *                     don't wait if the mutex object can't be unlocked                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully unlocked                                                         *
 *   E_FAILURE       : mutex object can't be unlocked                                                                 *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned mtx_give( mtx_t *mtx );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Mutex                                                                                          *
 *                                                                                                                    *
 * Description       : create and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Mutex : public __mtx
{
	explicit
	 Mutex( void ): __mtx _MTX_INIT() {}
	~Mutex( void ) { assert(owner == nullptr); }

	void     wait( void ) {        mtx_wait(this); }
	unsigned take( void ) { return mtx_take(this); }
	unsigned give( void ) { return mtx_give(this); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_MTX_H
