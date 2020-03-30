/******************************************************************************

    @file    IntrOS: osport.h
    @author  Rajmund Szymanski
    @date    29.03.2020
    @brief   IntrOS port definitions for X86.

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

#ifndef __INTROSPORT_H
#define __INTROSPORT_H

#include <stdint.h>
#include <time.h>
#ifndef   NOCONFIG
#include "osconfig.h"
#endif
#include "osdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifdef  CPU_FREQUENCY
#error  osconfig.h: CPU_FREAQUENCY value is independent of this software.
#endif

/* -------------------------------------------------------------------------- */

#ifdef  OS_FREQUENCY
#error  osconfig.h: OS_FREAQUENCY value is independent of this software.
#elif   defined(__linux__)
#define OS_FREQUENCY    1000000
#else
#define OS_FREQUENCY       1000
#endif

/* -------------------------------------------------------------------------- */
// !! WARNING! OS_TIMER_SIZE < HW_TIMER_SIZE may cause unexpected problems !!

#ifndef OS_TIMER_SIZE
#define OS_TIMER_SIZE        32 /* bit size of system timer counter           */
#endif

/* -------------------------------------------------------------------------- */
// !! WARNING! OS_TIMER_SIZE < HW_TIMER_SIZE may cause unexpected problems !!

#ifdef  HW_TIMER_SIZE
#error  HW_TIMER_SIZE is an internal definition!
#else
#define HW_TIMER_SIZE        32
#endif

/* -------------------------------------------------------------------------- */
// return current system time

#if HW_TIMER_SIZE >= OS_TIMER_SIZE

__STATIC_INLINE
clock_t port_sys_time( void )
{
	return clock();
}

#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROSPORT_H
