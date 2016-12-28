/******************************************************************************

    @file    IntrOS: os_tmr.c
    @author  Rajmund Szymanski
    @date    28.12.2016
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

#include <os.h>

/* -------------------------------------------------------------------------- */
static
void priv_tmr_start( tmr_id tmr )
/* -------------------------------------------------------------------------- */
{
	if (tmr->id == ID_STOPPED)
	core_tmr_insert(tmr);
}

/* -------------------------------------------------------------------------- */
void tmr_startUntil( tmr_id tmr, unsigned time, fun_id proc )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->state  = proc;
	tmr->start  = Counter;
	tmr->delay  = time - tmr->start;
	tmr->period = 0;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_start( tmr_id tmr, unsigned delay, unsigned period, fun_id proc )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->state  = proc;
	tmr->start  = Counter;
	tmr->delay  = delay;
	tmr->period = period;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tmr_take( tmr_id tmr )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(tmr);

	if (tmr->id == ID_STOPPED)
	{
		event = E_SUCCESS;
	}

	return event;
}

/* -------------------------------------------------------------------------- */
void tmr_wait( tmr_id tmr )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	if (tmr->id != ID_STOPPED)
	{
		unsigned signal = tmr->signal;
	
		while (tmr->signal == signal) tsk_yield();
	}
}

/* -------------------------------------------------------------------------- */
