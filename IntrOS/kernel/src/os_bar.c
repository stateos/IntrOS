/******************************************************************************

    @file    IntrOS: os_bar.c
    @author  Rajmund Szymanski
    @date    03.10.2017
    @brief   This file provides set of functions for IntrOS.

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

#include "inc/os_bar.h"

/* -------------------------------------------------------------------------- */
void bar_init( bar_t *bar, unsigned limit )
/* -------------------------------------------------------------------------- */
{
	assert(bar);
	assert(limit);

	port_sys_lock();

	memset(bar, 0, sizeof(bar_t));

	bar->count = limit;
	bar->limit = limit;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void bar_wait( bar_t *bar )
/* -------------------------------------------------------------------------- */
{
	unsigned signal;

	assert(bar);
	assert(bar->count);

	signal = bar->signal;

	port_sys_lock();

	if (--bar->count == 0)
	{
		bar->count = bar->limit;
		bar->signal++;
	}

	port_sys_unlock();

	while (bar->signal == signal) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
