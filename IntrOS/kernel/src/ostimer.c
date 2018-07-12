/******************************************************************************

    @file    IntrOS: ostimer.c
    @author  Rajmund Szymanski
    @date    11.07.2018
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

#include "inc/ostimer.h"

/* -------------------------------------------------------------------------- */
void tmr_init( tmr_t *tmr, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	core_sys_lock();

	memset(tmr, 0, sizeof(tmr_t));

	tmr->state = state;

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_tmr_start( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	if (tmr->id == ID_STOPPED)
		core_tmr_insert(tmr);
}

/* -------------------------------------------------------------------------- */
void tmr_startUntil( tmr_t *tmr, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	core_sys_lock();

	tmr->start  = core_sys_time();
	tmr->delay  = time - tmr->start;
	if (tmr->delay > ((CNT_MAX)>>1))
		tmr->delay = 0;
	tmr->period = 0;

	priv_tmr_start(tmr);

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_start( tmr_t *tmr, cnt_t delay, cnt_t period )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	core_sys_lock();

	tmr->start  = core_sys_time();
	tmr->delay  = delay;
	tmr->period = period;

	priv_tmr_start(tmr);

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_startFrom( tmr_t *tmr, cnt_t delay, cnt_t period, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	core_sys_lock();

	tmr->state  = proc;
	tmr->start  = core_sys_time();
	tmr->delay  = delay;
	tmr->period = period;

	priv_tmr_start(tmr);

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tmr_take( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(tmr);

	if (tmr->id == ID_STOPPED)
		event = E_SUCCESS;

	return event;
}

/* -------------------------------------------------------------------------- */
void tmr_wait( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	unsigned signal;

	assert(tmr);

	if (tmr->id != ID_STOPPED)
	{
		signal = tmr->signal;
		while (tmr->signal == signal) core_ctx_switch();
	}
}

/* -------------------------------------------------------------------------- */
