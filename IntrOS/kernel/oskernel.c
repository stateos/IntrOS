/******************************************************************************

    @file    IntrOS: oskernel.c
    @author  Rajmund Szymanski
    @date    18.04.2017
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

#include <os.h>

/* -------------------------------------------------------------------------- */

#ifndef MAIN_TOP
static  stk_t     MAIN_STACK[ASIZE(OS_STACK_SIZE)];
#define MAIN_TOP (MAIN_STACK+ASIZE(OS_STACK_SIZE))
#endif

tsk_t MAIN   = { .id=ID_READY, .prev=&MAIN, .next=&MAIN, .top=MAIN_TOP }; // main task
sys_t System = { .cur=&MAIN };

/* -------------------------------------------------------------------------- */

void core_rdy_insert( void *item, unsigned id, void *next )
{
	obj_t *obj = item;
	obj_t *nxt = next;
	obj_t *prv = nxt->prev;

	obj->id   = id;
	obj->prev = prv;
	obj->next = nxt;
	nxt->prev = obj;
	prv->next = obj;
}

/* -------------------------------------------------------------------------- */

void core_rdy_remove( void *item )
{
	obj_t *obj = item;
	obj_t *nxt = obj->next;
	obj_t *prv = obj->prev;

	nxt->prev = prv;
	prv->next = nxt;
	obj->id   = ID_STOPPED;
}

/* -------------------------------------------------------------------------- */

void core_ctx_init( tsk_t *tsk )
{
	port_ctx_init(&tsk->ctx.reg, tsk->top, core_tsk_loop);
}

/* -------------------------------------------------------------------------- */

void core_ctx_switch( void )
{
	port_sys_lock();

	if (setjmp(Current->ctx.buf) == 0)
		core_tsk_switch();

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

void core_tsk_loop( void )
{
	for (;;)
	{
		port_clr_lock();
		Current->state();
		core_ctx_switch();
	}
}

/* -------------------------------------------------------------------------- */

void core_tsk_switch( void )
{
	tsk_t *cur;
	tmr_t *tmr;

	for (;;)
	{
		port_isr_lock();

		cur = Current = Current->next;

		port_isr_unlock();

		if (cur->id == ID_STOPPED)
			continue;

		if (cur->id == ID_READY)
			break;

		if (cur->delay >= Counter - cur->start + 1)
			continue;

		if (cur->id == ID_DELAYED)
		{
			cur->id =  ID_READY;
			cur->event = E_SUCCESS;
			break;
		}
//		if (cur->id == ID_TIMER)
		{
			port_isr_lock();

			tmr = (tmr_t *) cur;
			
			tmr->start += tmr->delay;
			tmr->delay  = tmr->period;

			if (tmr->state)
			tmr->state();

			if (tmr->delay == 0)
			core_tmr_remove(tmr);

			tmr->signal++;

			port_isr_unlock();
		}
	}

	longjmp(cur->ctx.buf, 1);
}

/* -------------------------------------------------------------------------- */
