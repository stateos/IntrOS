/******************************************************************************

    @file    IntrOS: osport.c
    @author  Rajmund Szymanski
    @date    29.03.2020
    @brief   IntrOS port file for X86.

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

#include "oskernel.h"

/* -------------------------------------------------------------------------- */

void port_sys_init( void )
{
}

/* -------------------------------------------------------------------------- */

/******************************************************************************
 Tick-less mode: return current system time
*******************************************************************************/

#if HW_TIMER_SIZE < OS_TIMER_SIZE

cnt_t port_sys_time( void )
{
	static
	uint32_t clk = 0;
	uint32_t tck;
	cnt_t    cnt;

	cnt = System.cnt;
	tck = clock();

	if (tck < clk)
	{
		core_sys_tick();
		cnt = System.cnt;
	}

	clk = tck;

	return cnt + tck;
}

#endif

/******************************************************************************
 End of the function
*******************************************************************************/

/* -------------------------------------------------------------------------- */
