/******************************************************************************

    @file    IntrOS: os_evq.c
    @author  Rajmund Szymanski
    @date    02.05.2018
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

#include "inc/os_evq.h"

/* -------------------------------------------------------------------------- */
void evq_init( evq_t *evq, unsigned limit, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	assert(evq);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(evq, 0, sizeof(evq_t));

	evq->limit = limit;
	evq->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_get( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned event;
	unsigned f = evq->first;

	event = evq->data[f++];

	evq->first = (f < evq->limit) ? f : 0;
	evq->count--;

	return event;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_put( evq_t *evq, unsigned event )
/* -------------------------------------------------------------------------- */
{
	unsigned n = evq->next;
	
	evq->data[n++] = event;

	evq->next = (n < evq->limit) ? n : 0;
	evq->count++;
}

/* -------------------------------------------------------------------------- */
unsigned evq_take( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(evq);

	port_sys_lock();

	if (evq->count > 0)
		event = priv_evq_get(evq);

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_wait( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	while ((event = evq_take(evq)) == E_FAILURE) core_ctx_switch();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_give( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(evq);

	port_sys_lock();

	if (evq->count < evq->limit)
	{
		priv_evq_put(evq, data);
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evq_send( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	while (evq_give(evq, data) == E_FAILURE) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void evq_push( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	assert(evq);

	port_sys_lock();

	priv_evq_put(evq, data);

	if (evq->count > evq->limit)
	{
		evq->count = evq->limit;
		evq->first = evq->next;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
