/******************************************************************************

    @file    IntrOS: oskernel.c
    @author  Rajmund Szymanski
    @date    31.07.2018
    @brief   This file provides set of variables and functions for IntrOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#include "oskernel.h"
#include "inc/ostimer.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */

#ifndef MAIN_TOP
static  stk_t     MAIN_STK[SSIZE(OS_STACK_SIZE)];
#define MAIN_TOP (MAIN_STK+SSIZE(OS_STACK_SIZE))
#endif

tsk_t MAIN   = { .obj={ .next=&MAIN }, .id=ID_READY, .top=MAIN_TOP }; // main task
sys_t System = { .cur=&MAIN };

/* -------------------------------------------------------------------------- */

static
void priv_rdy_insert( obj_t *obj )
{
	obj_t *prv;

	if (obj->next == 0)
	{
		prv = obj->next = &MAIN;
		while (prv->next != &MAIN) prv = prv->next;
		prv->next = obj;
	}
}

/* -------------------------------------------------------------------------- */

void core_tmr_insert( tmr_t *tmr )
{
	tmr->id = ID_TIMER;
	priv_rdy_insert(&tmr->obj);
}

/* -------------------------------------------------------------------------- */

void core_tsk_insert( tsk_t *tsk )
{
	tsk->id = ID_READY;
	priv_rdy_insert(&tsk->obj);
}

/* -------------------------------------------------------------------------- */

void core_ctx_init( tsk_t *tsk )
{
#ifdef DEBUG
	memset(tsk->stack, 0xFF, (size_t)tsk->top - (size_t)tsk->stack);
#endif
	port_ctx_init(&tsk->ctx.reg, tsk->top, core_tsk_loop);
}

/* -------------------------------------------------------------------------- */

void core_ctx_switch( void )
{
	sys_lock();
	{
		if (setjmp(System.cur->ctx.buf) == 0)
			core_tsk_switch();
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */

void core_tsk_loop( void )
{
	for (;;)
	{
		port_clr_lock();
		System.cur->state();
		core_ctx_switch();
	}
}

/* -------------------------------------------------------------------------- */

static
bool priv_tmr_countdown( tmr_t *tmr )
{
	bool countdown = true;

	port_set_lock();
	{
		if ((cnt_t)(core_sys_time() - tmr->start + 1) > tmr->delay)
		{
			tmr->start += tmr->delay;
			tmr->delay  = tmr->period;
			countdown = false;
		}
	}
	port_clr_lock();

	return countdown;
}

/* -------------------------------------------------------------------------- */

void core_tsk_switch( void )
{
	tsk_t *cur;
	tmr_t *tmr;

	core_stk_assert();

	for (;;)
	{
		port_set_lock();
		{
			cur = System.cur = System.cur->obj.next;
		}
		port_clr_lock();

		if (cur->id == ID_STOPPED)
			continue;

		if (cur->id == ID_READY)
			break;

		if (priv_tmr_countdown((tmr_t *)cur))
			continue;

		if (cur->id == ID_DELAYED)
		{
			cur->id  = ID_READY;
			cur->event = E_SUCCESS;
			break;
		}

//		if (cur->id == ID_TIMER)
		{
			port_set_lock();
			{
				tmr = (tmr_t *)cur;

				if (tmr->state)
					tmr->state();

				if (tmr->delay == 0)
					tmr->id = ID_STOPPED;

				tmr->signal++;
			}
			port_clr_lock();
		}
	}

	longjmp(cur->ctx.buf, 1);
}

/* -------------------------------------------------------------------------- */
