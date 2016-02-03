/******************************************************************************

    @file    IntrOS: oskernel.c
    @author  Rajmund Szymanski
    @date    03.02.2016
    @brief   This file provides set of variables and functions for IntrOS.

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

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

#ifndef MAIN_SP
static  char     MAIN_STACK[ASIZE(OS_STACK_SIZE)] __osalign;
#define MAIN_SP &MAIN_STACK[ASIZE(OS_STACK_SIZE)]
#endif

static  tsk_t MAIN   = { ID_READY, &MAIN, &MAIN, 0, 0, 0, 0, 0, 0, MAIN_SP }; // main task

        sys_t System = { &MAIN };

/* -------------------------------------------------------------------------- */

__attribute__((noinline))
void core_rdy_insert( os_id obj, unsigned id, os_id nxt )
{
	obj_id Obj = obj;
	obj_id Nxt = nxt;
	obj_id Prv = Nxt->prev;

	Obj->id   = id;
	Obj->prev = Prv;
	Obj->next = Nxt;
	Nxt->prev = Obj;
	Prv->next = Obj;
}

/* -------------------------------------------------------------------------- */

__attribute__((noinline))
void core_rdy_remove( os_id obj )
{
	obj_id Obj = obj;
	obj_id Nxt = Obj->next;
	obj_id Prv = Obj->prev;

	Nxt->prev = Prv;
	Prv->next = Nxt;
	Obj->id   = ID_STOPPED;
}

/* -------------------------------------------------------------------------- */

tsk_id core_tsk_handler( void )
{
	tsk_id cur;

	for (;;)
	{
		port_set_lock();

		cur = Current = Current->next;

		port_clr_lock();

		if (cur->id == ID_STOPPED)
			continue;

		if (cur->id == ID_READY)
			break;

		if (cur->delay >= Counter - cur->start + 1)
			continue;

		if (cur->id == ID_DELAYED)
			break;

//		if (cur->id == ID_TIMER)
		{
			port_set_lock();

			tmr_id tmr = (tmr_id) cur;
			
			tmr->start += tmr->delay;
			tmr->delay  = tmr->period;

			if (tmr->state)
			tmr->state();

			if (tmr->delay == 0)
			core_tmr_remove(tmr);

			tmr->signal++;

			port_clr_lock();
		}
	}

	return cur;
}

/* -------------------------------------------------------------------------- */
