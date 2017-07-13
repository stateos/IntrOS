/******************************************************************************

    @file    IntrOS: os_evt.h
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

#ifndef __INTROS_EVT_H
#define __INTROS_EVT_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : event                                                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __evt evt_t, * const evt_id;

struct __evt
{
	unsigned signal;
	unsigned event;
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _EVT_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize an event object                                                           *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : event object                                                                                   *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _EVT_INIT() { 0, 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_EVT                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize an event object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : name of a pointer to event object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_EVT( evt )                     \
                       evt_t evt##__evt = _EVT_INIT(); \
                       evt_id evt = & evt##__evt

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_EVT                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static event object                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : name of a pointer to event object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_EVT( evt )                     \
                static evt_t evt##__evt = _EVT_INIT(); \
                static evt_id evt = & evt##__evt

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : EVT_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize an event object                                                           *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : event object                                                                                   *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                EVT_INIT() \
                      _EVT_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : EVT_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize an event object                                                           *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to event object                                                                        *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                EVT_CREATE() \
             & (evt_t) EVT_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the event object has been released                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : pointer to event object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   'event'         : event object was successfully released with the 'event' value                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned evt_wait( evt_t *evt );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : evt_give                                                                                       *
 *                                                                                                                    *
 * Description       : resume all tasks that are waiting on the event object                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : pointer to event object                                                                        *
 *   event           : all waiting tasks are resumed with the 'event' value                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void evt_give( evt_t *evt, unsigned event );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Event                                                                                          *
 *                                                                                                                    *
 * Description       : create and initilize an event object                                                           *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Event : public __evt
{
	explicit
	Event( void ): __evt _EVT_INIT() {}

	unsigned wait( void )            { return evt_wait(this);         }
	void     give( unsigned _event ) {        evt_give(this, _event); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_EVT_H
