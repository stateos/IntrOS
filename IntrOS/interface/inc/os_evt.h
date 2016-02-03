/******************************************************************************

    @file    IntrOS: os_evt.h
    @author  Rajmund Szymanski
    @date    03.02.2016
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

#pragma once

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : event                                                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_EVT                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a event object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   evt             : name of a pointer to event object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define     OS_EVT( evt )                     \
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

#define static_EVT( evt )                     \
        static evt_t evt##__evt = _EVT_INIT(); \
        static evt_id evt = & evt##__evt

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

#define                EVT_INIT( ) \
                      _EVT_INIT()

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
#define                EVT_CREATE( ) \
               &(evt_t)EVT_INIT()
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

              unsigned evt_wait( evt_id evt );

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

              void     evt_give( evt_id evt, unsigned event );

#ifdef __cplusplus
}
#endif
