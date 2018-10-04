/******************************************************************************

    @file    IntrOS: ostask.c
    @author  Rajmund Szymanski
    @date    04.10.2018
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

#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void tsk_init( tsk_t *tsk, fun_t *state, stk_t *stack, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(state);
	assert(stack);
	assert(size);

	sys_lock();
	{
		memset(tsk, 0, sizeof(tsk_t));

		core_hdr_init(&tsk->hdr);

		tsk->state = state;
		tsk->stack = stack;
		tsk->size  = size;

		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_start( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(tsk->state);

	sys_lock();
	{
		if (tsk->hdr.id == ID_STOPPED)
		{
			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_startFrom( tsk_t *tsk, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(state);

	sys_lock();
	{
		if (tsk->hdr.id == ID_STOPPED)
		{
			tsk->state = state;

			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();
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
void tsk_kill( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	sys_lock();
	{
		core_tsk_remove(tsk);
		if (tsk == System.cur)
			core_ctx_switch();
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_join( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	while (tsk->hdr.id != ID_STOPPED)
		core_ctx_switch();
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
void priv_tsk_sleep( tsk_t *cur )
/* -------------------------------------------------------------------------- */
{
	cur->hdr.id = ID_BLOCKED;
	core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void tsk_sleepFor( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	sys_lock();
	{
		cur->start = core_sys_time();
		cur->delay = delay;

		priv_tsk_sleep(cur);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_sleepNext( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	sys_lock();
	{
		cur->delay = delay;

		priv_tsk_sleep(cur);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_sleepUntil( cnt_t time )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	sys_lock();
	{
		cur->start = core_sys_time();
		cur->delay = time - cur->start;
		if (cur->delay > ((CNT_MAX)>>1))
			cur->delay = 0;

		priv_tsk_sleep(cur);
	}
	sys_unlock();
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

	sys_lock();
	{
		if (tsk->hdr.id == ID_READY)
			tsk->event &= ~flags;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_suspend( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	if (tsk->hdr.id != ID_READY)
		return E_FAILURE;

	tsk->hdr.id = ID_BLOCKED;
	tsk->delay = INFINITE;
	if (tsk == System.cur)
		core_ctx_switch();
	return E_SUCCESS;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_resume( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	if (tsk->hdr.id != ID_BLOCKED || tsk->delay != INFINITE)
		return E_FAILURE;

	tsk->hdr.id = ID_READY;
	return E_SUCCESS;
}

/* -------------------------------------------------------------------------- */
