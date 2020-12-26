/******************************************************************************

    @file    IntrOS: oseventqueue.c
    @author  Rajmund Szymanski
    @date    26.12.2020
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

#include "inc/oseventqueue.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void evq_init( evq_t *evq, unsigned *data, size_t bufsize )
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
unsigned priv_evq_get( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->head;

	unsigned event = evq->data[i++];

	evq->head = (i < evq->limit) ? i : 0;
	evq->count--;

	return event;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_put( evq_t *evq, const unsigned event )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->tail;

	evq->data[i++] = event;

	evq->tail = (i < evq->limit) ? i : 0;
	evq->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_skip( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->head + 1;

	evq->head = (i < evq->limit) ? i : 0;
	evq->count--;
}

/* -------------------------------------------------------------------------- */
unsigned evq_take( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(evq);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		if (evq->count > 0)
			result = priv_evq_get(evq);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned evq_wait( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned result;

	while (result = evq_take(evq), result == FAILURE) core_ctx_switch();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned evq_give( evq_t *evq, unsigned event )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(evq);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		if (evq->count < evq->limit)
		{
			priv_evq_put(evq, event);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void evq_send( evq_t *evq, unsigned event )
/* -------------------------------------------------------------------------- */
{
	while (evq_give(evq, event) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void evq_push( evq_t *evq, unsigned event )
/* -------------------------------------------------------------------------- */
{
	assert(evq);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		if (evq->count == evq->limit)
			priv_evq_skip(evq);
		priv_evq_put(evq, event);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned evq_count( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned count;

	assert(evq);

	sys_lock();
	{
		count = evq->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
unsigned evq_space( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned space;

	assert(evq);

	sys_lock();
	{
		space = evq->limit - evq->count;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
unsigned evq_limit( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned limit;

	assert(evq);

	sys_lock();
	{
		limit = evq->limit;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */

#if OS_ATOMICS

/* -------------------------------------------------------------------------- */
static
unsigned priv_evq_getAsync( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->head;

	unsigned event = evq->data[i++];

	evq->head = (i < evq->limit) ? i : 0;
	atomic_fetch_sub((atomic_uint *)&evq->count, 1);

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned evq_takeAsync( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(evq);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		if (atomic_load((atomic_uint *)&evq->count) > 0)
			result = priv_evq_getAsync(evq);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned evq_waitAsync( evq_t *evq )
/* -------------------------------------------------------------------------- */
{
	unsigned result;

	while (result = evq_takeAsync(evq), result == FAILURE) core_ctx_switch();

	return result;
}

/* -------------------------------------------------------------------------- */
static
void priv_evq_putAsync( evq_t *evq, const unsigned event )
/* -------------------------------------------------------------------------- */
{
	unsigned i = evq->tail;

	evq->data[i++] = event;

	evq->tail = (i < evq->limit) ? i : 0;
	atomic_fetch_add((atomic_uint *)&evq->count, 1);
}

/* -------------------------------------------------------------------------- */
unsigned evq_giveAsync( evq_t *evq, unsigned event )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(evq);
	assert(evq->data);
	assert(evq->limit);

	sys_lock();
	{
		if (atomic_load((atomic_uint *)&evq->count) < evq->limit)
		{
			priv_evq_putAsync(evq, event);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void evq_sendAsync( evq_t *evq, unsigned event )
/* -------------------------------------------------------------------------- */
{
	while (evq_giveAsync(evq, event) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */

#endif//OS_ATOMICS
