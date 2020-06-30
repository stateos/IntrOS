/******************************************************************************

    @file    IntrOS: osevent.c
    @author  Rajmund Szymanski
    @date    30.06.2020
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

#include "inc/osevent.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void evt_init( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	assert(evt);

	sys_lock();
	{
		memset(evt, 0, sizeof(evt_t));
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned evt_take( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(evt);

	sys_lock();
	{
		event = evt->event;
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evt_wait( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	unsigned event;
	unsigned signal;

	assert(evt);

	sys_lock();
	{
		signal = evt->signal;
		while (evt->signal == signal) core_ctx_switch();
		event = evt->event;
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evt_give( evt_t *evt, unsigned event )
/* -------------------------------------------------------------------------- */
{
	assert(evt);

	sys_lock();
	{
		evt->event = event;
		evt->signal++;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
