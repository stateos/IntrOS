/******************************************************************************

    @file    IntrOS: oseventqueue.c
    @author  Rajmund Szymanski
    @date    08.09.2018
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
void evq_init( evq_t *evq, unsigned *data, unsigned bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(evq);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(evq, 0, sizeof(evq_t));

		evq->limit = bufsize / sizeof(unsigned);
		evq->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_get( evq_t *evq, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->head;

	*data = evq->data[i++];

	evq->head = (i < evq->limit) ? i : 0;
	evq->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_put( evq_t *evq, const unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->tail;

	evq->data[i++] = data;

	evq->tail = (i < evq->limit) ? i : 0;
	evq->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_skip( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	evq->count--;
	evq->head++;
	if (evq->head == evq->limit) evq->head = 0;
}

/* -------------------------------------------------------------------------- */
unsigned evq_take( evq_t *evq, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(evq);

	sys_lock();
	{
		if (evq->count > 0)
		{
			priv_evq_get(evq, data);
			event = E_SUCCESS;
		}
		else
		{
			event = E_FAILURE;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evq_wait( evq_t *evq, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	while (evq_take(evq, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned evq_give( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(evq);

	sys_lock();
	{
		if (evq->count < evq->limit)
		{
			priv_evq_put(evq, data);
			event = E_SUCCESS;
		}
		else
		{
			event = E_FAILURE;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void evq_send( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	while (evq_give(evq, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void evq_push( evq_t *evq, unsigned data )
/* -------------------------------------------------------------------------- */
{
	assert(evq);

	sys_lock();
	{
		if (evq->count == evq->limit)
			priv_evq_skip(evq);
		priv_evq_put(evq, data);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
