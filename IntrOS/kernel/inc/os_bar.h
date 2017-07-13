/******************************************************************************

    @file    IntrOS: os_bar.h
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

#ifndef __INTROS_BAR_H
#define __INTROS_BAR_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : barrier                                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __bar bar_t, * const bar_id;

struct __bar
{
	unsigned signal;
	unsigned count; // barrier's current value
	unsigned limit; // barrier's value limit
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _BAR_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : barrier object                                                                                 *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _BAR_INIT( limit ) { 0, limit, limit }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_BAR                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : name of a pointer to barrier object                                                            *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_BAR( bar, limit )                     \
                       bar_t bar##__bar = _BAR_INIT( limit ); \
                       bar_id bar = & bar##__bar

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_BAR                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static barrier object                                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : name of a pointer to barrier object                                                            *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_BAR( bar, limit )                     \
                static bar_t bar##__bar = _BAR_INIT( limit ); \
                static bar_id bar = & bar##__bar

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BAR_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : barrier object                                                                                 *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                BAR_INIT( limit ) \
                      _BAR_INIT( limit )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BAR_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : pointer to barrier object                                                                      *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                BAR_CREATE( limit ) \
             & (bar_t) BAR_INIT( limit )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : bar_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the barrier object has been released                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : pointer to barrier object                                                                      *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void bar_wait( bar_t *bar );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Barrier                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : number of tasks that must call wait function to release the barrier object                     *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Barrier : public __bar
{
	explicit
	Barrier( const unsigned _limit ): __bar _BAR_INIT(0) { limit = _limit; }

	void wait( void ) { bar_wait(this); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_BAR_H
