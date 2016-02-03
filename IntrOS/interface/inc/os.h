/******************************************************************************

    @file    IntrOS: os.h
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
#include <os_evt.h> // event
#include <os_flg.h> // flag
#include <os_bar.h> // barrier
#include <os_sem.h> // semaphore
#include <os_mtx.h> // mutex
#include <os_cnd.h> // condition variable
#include <os_box.h> // mailbox queue
#include <os_msg.h> // message queue
#include <os_tmr.h> // timer
#include <os_tsk.h> // task

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_init                                                                                       *
 *                                                                                                                    *
 * Description       : initialize system timer and enabled services                                                   *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : call sys_init as the first instruction in function main                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     sys_init( void ) { port_sys_init(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_lock                                                                                       *
 *                                                                                                                    *
 * Description       : disable interrupts / enter into critical section                                               *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                sys_lock()         port_sys_lock()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sys_unlock                                                                                     *
 *                                                                                                                    *
 * Description       : enable interrupts / exit from critical section                                                 *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                sys_unlock()       port_sys_unlock()

#ifdef __cplusplus
}
#endif
