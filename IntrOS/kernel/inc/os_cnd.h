/******************************************************************************

    @file    IntrOS: os_cnd.h
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

#ifndef __INTROS_CND_H
#define __INTROS_CND_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : condition variable                                                                             *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __cnd cnd_t, * const cnd_id;

struct __cnd
{
	unsigned signal;
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _CND_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : condition variable object                                                                      *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _CND_INIT() { 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_CND                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : name of a pointer to condition variable object                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_CND( cnd )                     \
                       cnd_t cnd##__cnd = _CND_INIT(); \
                       cnd_id cnd = & cnd##__cnd

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_CND                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static condition variable object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : name of a pointer to condition variable object                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_CND( cnd )                     \
                static cnd_t cnd##__cnd = _CND_INIT(); \
                static cnd_id cnd = & cnd##__cnd

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : CND_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : condition variable object                                                                      *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                CND_INIT() \
                      _CND_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : CND_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to condition variable object                                                           *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                CND_CREATE() \
             & (cnd_t) CND_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly on the condition variable releasing the currently owned mutex,                *
 *                     and finally lock the mutex again                                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : pointer to condition variable object                                                           *
 *   mtx             : currently owned mutex                                                                          *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void cnd_wait( cnd_t *cnd, mtx_t *mtx );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : cnd_give                                                                                       *
 *                                                                                                                    *
 * Description       : signal one or all tasks that are waiting on the condition variable                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   cnd             : pointer to condition variable object                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void cnd_give( cnd_t *cnd );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : ConditionVariable                                                                              *
 *                                                                                                                    *
 * Description       : create and initilize a condition variable object                                               *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct ConditionVariable : public __cnd
{
	explicit
	ConditionVariable( void ): __cnd _CND_INIT() {}

	void wait( mtx_t *_mtx ) { cnd_wait(this, _mtx); }
	void give( void )        { cnd_give(this);       }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_CND_H
