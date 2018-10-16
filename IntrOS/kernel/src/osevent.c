/******************************************************************************

    @file    IntrOS: osevent.c
    @author  Rajmund Szymanski
    @date    16.10.2018
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
void evt_wait( evt_t *evt, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned signal;

	assert(evt);
	assert(data);

	sys_lock();
	{
		signal = evt->signal;
		do core_ctx_switch(); while (evt->signal == signal);
		*data = evt->event;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void evt_give( evt_t *evt, unsigned data )
/* -------------------------------------------------------------------------- */
{
	assert(evt);

	sys_lock();
	{
		evt->event = data;
		evt->signal++;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
