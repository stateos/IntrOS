/******************************************************************************

    @file    IntrOS: os_tsk.c
    @author  Rajmund Szymanski
    @date    12.04.2017
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
void tsk_start( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(tsk->state);

	port_sys_lock();

	if (tsk->id == ID_STOPPED)
	{
		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_startFrom( tsk_t *tsk, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(state);

	port_sys_lock();

	if (tsk->id == ID_STOPPED)
	{
		tsk->state = state;

		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_stop( void )
/* -------------------------------------------------------------------------- */
{
	port_set_lock();

	core_tsk_remove(Current);
	core_tsk_switch();
}

/* -------------------------------------------------------------------------- */
void tsk_join( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	while (tsk->id != ID_STOPPED) tsk_yield();
}

/* -------------------------------------------------------------------------- */
void tsk_flip( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(state);

	Current->state = state;

	core_ctx_init(Current);
	core_tsk_switch();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_tsk_sleep( tsk_t *cur )
/* -------------------------------------------------------------------------- */
{
	cur->id = ID_DELAYED;
	tsk_yield();

	return cur->event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepUntil( unsigned time )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = Current;

	cur->start = Counter;
	cur->delay = time - cur->start;

	return priv_tsk_sleep(cur);
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepFor( unsigned delay )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = Current;

	cur->start = Counter;
	cur->delay = delay;

	return priv_tsk_sleep(cur);
}

/* -------------------------------------------------------------------------- */
void tsk_wait( unsigned flags )
/* -------------------------------------------------------------------------- */
{
	Current->event = flags;
	while (Current->event) tsk_yield();
}

/* -------------------------------------------------------------------------- */
void tsk_give( tsk_t *tsk, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	if (tsk->id == ID_READY)
	{
		port_sys_lock();
		tsk->event &= ~flags;
		port_sys_unlock();
	}
}

/* -------------------------------------------------------------------------- */
unsigned tsk_suspend( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(tsk);

	if (tsk->id == ID_READY)
	{
		tsk->delay = INFINITE;
		tsk->id = ID_DELAYED;
		if (tsk == Current)
			tsk_yield();
		event = E_SUCCESS;
	}

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_resume( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(tsk);

	if (tsk->id == ID_DELAYED)
	{
		tsk->event = E_FAILURE;
		tsk->id = ID_READY;
		event = E_SUCCESS;
	}

	return event;
}

/* -------------------------------------------------------------------------- */
