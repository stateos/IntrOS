/******************************************************************************

    @file    IntrOS: ostask.c
    @author  Rajmund Szymanski
    @date    13.12.2020
    @brief   This file provides set of functions for IntrOS.

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

#include "inc/ostask.h"
#include "inc/ossignal.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
static
void priv_wrk_init( tsk_t *tsk, fun_t *state, stk_t *stack, size_t size )
/* -------------------------------------------------------------------------- */
{
	memset(tsk, 0, sizeof(tsk_t));

	core_hdr_init(&tsk->hdr);

	tsk->state = state;
	tsk->stack = stack;
	tsk->size  = size;
}

/* -------------------------------------------------------------------------- */
void wrk_init( tsk_t *tsk, fun_t *state, stk_t *stack, size_t size )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(stack);
	assert(size);

	sys_lock();
	{
		priv_wrk_init(tsk, state, stack, size);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_init( tsk_t *tsk, fun_t *state, stk_t *stack, size_t size )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(state);
	assert(stack);
	assert(size);

	sys_lock();
	{
		priv_wrk_init(tsk, state, stack, size);
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
		if (tsk->hdr.id == ID_STOPPED)  // active tasks cannot be started
		{
			tsk->state = state;

			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_tsk_reset( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	tsk->delay = 0;
	tsk->sig.sigset = 0;
	tsk->sig.backup.pc = 0;
}

/* -------------------------------------------------------------------------- */
void tsk_stop( void )
/* -------------------------------------------------------------------------- */
{
	port_set_lock();

	priv_tsk_reset(System.cur);
	core_tsk_remove(System.cur);
	core_tsk_switch();
}

/* -------------------------------------------------------------------------- */
void tsk_reset( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	sys_lock();
	{
		priv_tsk_reset(tsk);
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

	priv_tsk_reset(System.cur);
	core_ctx_init(System.cur);
	core_tsk_switch();
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

		core_ctx_switch();
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

		core_ctx_switch();
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
		if (cur->delay > CNT_LIMIT)
			cur->delay = 0;

		core_ctx_switch();
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_suspend( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	if (tsk->hdr.id != ID_READY || tsk->delay != 0)
		return FAILURE;

	tsk->delay = INFINITE;
	if (tsk == System.cur)
		core_ctx_switch();
	return SUCCESS;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_resume( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	if (tsk->hdr.id != ID_READY || tsk->delay != INFINITE)
		return FAILURE;

	tsk->delay = 0;
	return SUCCESS;
}

/* -------------------------------------------------------------------------- */
static
void priv_sig_handler( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned signo;
	unsigned sigset;
	act_t  * action = tsk->sig.action;

	while (tsk->sig.sigset)
	{
		sigset = tsk->sig.sigset;
		sigset &= -sigset;
		tsk->sig.sigset &= ~sigset;

		port_clr_lock();
		{
			if (action)
			{
				for (signo = 0; sigset >>= 1; signo++);
				action(signo);
			}
		}
		port_set_lock();
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_sig_deliver( void )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk = System.cur;

	port_set_lock();

	priv_sig_handler(tsk);

	tsk->ctx.reg.pc = tsk->sig.backup.pc;
	tsk->sig.backup.pc = 0;
	tsk->delay = tsk->sig.backup.delay;

	core_tsk_switch();
}

/* -------------------------------------------------------------------------- */
static
void priv_sig_dispatch( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	if (tsk->sig.backup.pc)
		return;

	if (tsk == System.cur)
	{
		priv_sig_handler(tsk);
		return;
	}

	tsk->sig.backup.pc = tsk->ctx.reg.pc;
	tsk->sig.backup.delay = tsk->delay;

	tsk->ctx.reg.pc = priv_sig_deliver;
	tsk->delay = 0;
}

/* -------------------------------------------------------------------------- */
void tsk_give( tsk_t *tsk, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned sigset = SIGSET(signo);

	assert(tsk);
	assert(sigset);

	sys_lock();
	{
		if (tsk->hdr.id == ID_READY)
		{
			tsk->sig.sigset |= sigset;
			if (tsk->sig.sigset && tsk->sig.action)
				priv_sig_dispatch(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_action( tsk_t *tsk, act_t *action )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	sys_lock();
	{
		if (tsk->hdr.id == ID_READY)
		{
			tsk->sig.action = action;
			if (tsk->sig.action && tsk->sig.sigset)
				priv_sig_dispatch(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
