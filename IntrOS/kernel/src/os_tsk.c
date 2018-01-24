/******************************************************************************

    @file    IntrOS: os_tsk.c
    @author  Rajmund Szymanski
    @date    24.01.2018
    @brief   This file provides set of functions for IntrOS.

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

#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void tsk_init( tsk_t *tsk, fun_t *state, void *stack, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(state);
	assert(stack);
	assert(size);

	port_sys_lock();

	memset(tsk, 0, sizeof(tsk_t));
	
	tsk->state = state;
	tsk->stack = stack;
	tsk->top   = (stk_t *) LIMITED((char *)stack + size, stk_t);

	core_ctx_init(tsk);
	core_tsk_insert(tsk);

	port_sys_unlock();
}

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

	core_tsk_remove(System.cur);
	core_tsk_switch();
}

/* -------------------------------------------------------------------------- */
void tsk_join( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	while (tsk->id != ID_STOPPED) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void tsk_flip( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(state);

	System.cur->state = state;

	core_ctx_init(System.cur);
	core_tsk_switch();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_tsk_sleep( tsk_t *cur )
/* -------------------------------------------------------------------------- */
{
	cur->id = ID_DELAYED;
	core_ctx_switch();

	return cur->event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepUntil( cnt_t time )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	port_sys_lock();

	cur->start = core_sys_time();
	cur->delay = time - cur->start;
	if (cur->delay > ((CNT_MAX)>>1))
		cur->delay = 0;

	port_sys_unlock();

	return priv_tsk_sleep(cur);
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepFor( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	port_sys_lock();

	cur->start = core_sys_time();
	cur->delay = delay;

	port_sys_unlock();

	return priv_tsk_sleep(cur);
}

/* -------------------------------------------------------------------------- */
void tsk_wait( unsigned flags )
/* -------------------------------------------------------------------------- */
{
	System.cur->event = flags;
	while (System.cur->event) core_ctx_switch();
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
		if (tsk == System.cur)
			core_ctx_switch();
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
