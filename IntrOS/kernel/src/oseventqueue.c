/******************************************************************************

    @file    IntrOS: oseventqueue.c
    @author  Rajmund Szymanski
    @date    27.08.2018
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

#include "inc/oseventqueue.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void evt_init( evt_t *evt, unsigned *data, unsigned bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(evt);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(evt, 0, sizeof(evt_t));

		evt->limit = bufsize / sizeof(unsigned);
		evt->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_get( evt_t *evt, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evt->head;

	*data = evt->data[i++];

	evt->head = (i < evt->limit) ? i : 0;
	evt->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_put( evt_t *evt, const unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evt->tail;

	evt->data[i++] = data;

	evt->tail = (i < evt->limit) ? i : 0;
	evt->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_evt_skip( evt_t *evt )
/* -------------------------------------------------------------------------- */
{
	evt->count--;
	evt->head++;
	if (evt->head == evt->limit) evt->head = 0;
}

/* -------------------------------------------------------------------------- */
unsigned evt_take( evt_t *evt, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(evt);

	sys_lock();
	{
		if (evt->count > 0)
		{
			priv_evt_get(evt, data);
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evt_wait( evt_t *evt, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	while (evt_take(evt, data) == E_FAILURE) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned evt_give( evt_t *evt, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(evt);

	sys_lock();
	{
		if (evt->count < evt->limit)
		{
			priv_evt_put(evt, data);
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evt_send( evt_t *evt, unsigned data )
/* -------------------------------------------------------------------------- */
{
	while (evt_give(evt, data) == E_FAILURE) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void evt_push( evt_t *evt, unsigned data )
/* -------------------------------------------------------------------------- */
{
	assert(evt);

	sys_lock();
	{
		if (evt->count == evt->limit)
			priv_evt_skip(evt);
		priv_evt_put(evt, data);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
