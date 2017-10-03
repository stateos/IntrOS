/******************************************************************************

    @file    IntrOS: os_evt.c
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

#include "inc/os_evt.h"

/* -------------------------------------------------------------------------- */
void evt_init( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	assert(evt);

	port_sys_lock();

	memset(evt, 0, sizeof(evt_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned evt_wait( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	unsigned signal;

	assert(evt);

	signal = evt->signal;
	while (evt->signal == signal) core_ctx_switch();

	return evt->event;
}

/* -------------------------------------------------------------------------- */
void evt_give( evt_t *evt, unsigned event )
/* -------------------------------------------------------------------------- */
{
	assert(evt);

	port_sys_lock();

	evt->event = event;
	evt->signal++;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
