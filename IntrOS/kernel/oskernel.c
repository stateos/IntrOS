/******************************************************************************

    @file    IntrOS: oskernel.c
    @author  Rajmund Szymanski
    @date    17.12.2020
    @brief   This file provides set of variables and functions for IntrOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

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
// SYSTEM TIMER SERVICES
/* -------------------------------------------------------------------------- */

void core_tmr_insert( tmr_t *tmr )
{
	tsk_t *nxt = System.cur;
	tsk_t *prv = nxt->hdr.prev;

	tmr->hdr.id = ID_TIMER;

	tmr->hdr.prev = prv;
	tmr->hdr.next = nxt;
	nxt->hdr.prev = tmr;
	prv->hdr.next = tmr;
}

/* -------------------------------------------------------------------------- */

void core_tmr_remove( tmr_t *tmr )
{
	tsk_t *prv = tmr->hdr.prev;
	tsk_t *nxt = tmr->hdr.next;

	tmr->hdr.id = ID_STOPPED;

	nxt->hdr.prev = prv;
	prv->hdr.next = nxt;
}

/* -------------------------------------------------------------------------- */
// SYSTEM TASK SERVICES
/* -------------------------------------------------------------------------- */

#ifndef MAIN_TOP
static  stk_t     MAIN_STK[STK_SIZE(OS_STACK_SIZE)] __STKALIGN;
#define MAIN_TOP (MAIN_STK+STK_SIZE(OS_STACK_SIZE))
#endif

tsk_t MAIN = { .hdr={ .prev=&MAIN, .next=&MAIN, .id=ID_READY }, .stack=MAIN_TOP }; // main task
sys_t System = { .cur=&MAIN };

/* -------------------------------------------------------------------------- */

void core_tsk_insert( tsk_t *tsk )
{
	tsk_t *nxt = System.cur;
	tsk_t *prv = nxt->hdr.prev;

	tsk->hdr.id = ID_READY;

	tsk->hdr.prev = prv;
	tsk->hdr.next = nxt;
	nxt->hdr.prev = tsk;
	prv->hdr.next = tsk;
}

/* -------------------------------------------------------------------------- */

void core_tsk_remove( tsk_t *tsk )
{
	tsk_t *prv = tsk->hdr.prev;
	tsk_t *nxt = tsk->hdr.next;

	tsk->hdr.id = ID_STOPPED;

	nxt->hdr.prev = prv;
	prv->hdr.next = nxt;
}

/* -------------------------------------------------------------------------- */

void core_ctx_init( tsk_t *tsk )
{
	assert(tsk->size>STK_OVER(0));
#ifdef DEBUG
	if (tsk != System.cur)
		memset(tsk->stack, 0xFF, tsk->size);
#endif
#if OS_TASK_EXIT == 0
	port_ctx_init(&tsk->ctx.reg, (stk_t *)STK_CROP(tsk->stack, tsk->size), core_tsk_loop);
#else
	port_ctx_init(&tsk->ctx.reg, (stk_t *)STK_CROP(tsk->stack, tsk->size), core_tsk_exec);
#endif
}

/* -------------------------------------------------------------------------- */
#ifdef DEBUG

size_t core_stk_space( tsk_t *tsk )
{
	void *stk = tsk->stack;
	uint8_t *ptr = stk;
	while (*ptr == 0xFF) ptr++;
	return (uintptr_t)ptr - (uintptr_t)stk;
}

static
bool priv_stk_integrity( tsk_t *tsk, void *sp )
{
	void *tp = tsk->stack + STK_SIZE(0);
	if (tsk == &MAIN) return true;
	if (sp < tp) return false;
#if (__MPU_USED == 0) && ((OS_GUARD_SIZE) > 0)
	if (core_stk_space(tsk) < STK_OVER(0)) return false;
#endif
	return true;
}

bool core_ctx_integrity( tsk_t *tsk )
{
	void *sp = tsk->ctx.reg.sp;
	return priv_stk_integrity(tsk, sp);
}

bool core_stk_integrity( void )
{
	tsk_t *tsk = System.cur;
	void *sp = port_get_sp();
	return priv_stk_integrity(tsk, sp);
}

#endif
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

void core_tsk_exec( void )
{
	port_clr_lock();
	System.cur->state();
	tsk_stop();
}

/* -------------------------------------------------------------------------- */

static
bool priv_tmr_countdown( tmr_t *tmr )
{
	bool countdown = true;

	port_set_lock();
	{
		if (core_sys_time() - tmr->start + 1 > tmr->delay)
		{
			tmr->start += tmr->delay;
			tmr->delay = tmr->period;
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

	assert_ctx_integrity(System.cur);

	for (;;)
	{
		port_set_lock();
		{
			cur = System.cur = System.cur->hdr.next;
		}
		port_clr_lock();

		if (cur->hdr.id == ID_STOPPED)
			continue;

		if (cur->delay && priv_tmr_countdown((tmr_t *)cur))
			continue;

		if (cur->hdr.id == ID_READY)
			break;

//		if (cur->hdr.id == ID_TIMER)
		{
			port_set_lock();
			{
				tmr = (tmr_t *)cur;

				if (tmr->state)
					tmr->state();

				if (tmr->delay == 0)
					core_tmr_remove(tmr);

				tmr->signal++;
			}
			port_clr_lock();
		}
	}

#if __MPU_USED == 1
//	port_mpu_disable();
	port_mpu_stackUpdate(cur == &MAIN ? NULL : cur->stack);
//	port_mpu_enable();
#endif

	assert_ctx_integrity(System.cur);

	longjmp(cur->ctx.buf, 1);
}

/* -------------------------------------------------------------------------- */
