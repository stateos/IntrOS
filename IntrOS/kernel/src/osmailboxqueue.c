/******************************************************************************

    @file    IntrOS: osmailboxqueue.c
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

#include "inc/osmailboxqueue.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void box_init( box_t *box, size_t size, void *data, size_t bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(box);
	assert(size);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(box, 0, sizeof(box_t));

		box->limit = (bufsize / size) * size;
		box->size  = size;
		box->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_box_get( box_t *box, char *data )
/* -------------------------------------------------------------------------- */
{
	size_t size = box->size;
	size_t i = box->head;

	while (size--)
		*data++ = box->data[i++];
	box->head = (i < box->limit) ? i : 0;
	box->count -= box->size;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_put( box_t *box, const char *data )
/* -------------------------------------------------------------------------- */
{
	size_t size = box->size;
	size_t i = box->tail;

	while (size--)
		box->data[i++] = *data++;
	box->tail = (i < box->limit) ? i : 0;
	box->count += box->size;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_skip( box_t *box )
/* -------------------------------------------------------------------------- */
{
	size_t i = box->head + box->size;

	box->head = (i < box->limit) ? i : 0;
	box->count -= box->size;
}

/* -------------------------------------------------------------------------- */
unsigned box_take( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(box);
	assert(box->data);
	assert(box->limit);
	assert(data);

	sys_lock();
	{
		if (box->count > 0)
		{
			priv_box_get(box, data);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void box_wait( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_take(box, data) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned box_give( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(box);
	assert(box->data);
	assert(box->limit);
	assert(data);

	sys_lock();
	{
		if (box->count < box->limit)
		{
			priv_box_put(box, data);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void box_send( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_give(box, data) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void box_push( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	assert(box);
	assert(box->data);
	assert(box->limit);
	assert(data);

	sys_lock();
	{
		if (box->count == box->limit)
			priv_box_skip(box);
		priv_box_put(box, data);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned box_count( box_t *box )
/* -------------------------------------------------------------------------- */
{
	unsigned count;

	assert(box);

	sys_lock();
	{
		count = box->count / box->size;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
unsigned box_space( box_t *box )
/* -------------------------------------------------------------------------- */
{
	unsigned space;

	assert(box);

	sys_lock();
	{
		space = (box->limit - box->count) / box->size;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
unsigned box_limit( box_t *box )
/* -------------------------------------------------------------------------- */
{
	unsigned limit;

	assert(box);

	sys_lock();
	{
		limit = box->limit / box->size;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */

#if OS_ATOMICS

/* -------------------------------------------------------------------------- */
static
void priv_box_getAsync( box_t *box, char *data )
/* -------------------------------------------------------------------------- */
{
	size_t size = box->size;
	size_t i = box->head;

	while (size--)
		*data++ = box->data[i++];
	box->head = (i < box->limit) ? i : 0;
	atomic_fetch_sub((atomic_uint *)&box->count, box->size);
}

/* -------------------------------------------------------------------------- */
unsigned box_takeAsync( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(box);
	assert(box->data);
	assert(box->limit);
	assert(data);

	sys_lock();
	{
		if (atomic_load((atomic_uint *)&box->count) > 0)
		{
			priv_box_getAsync(box, data);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void box_waitAsync( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_takeAsync(box, data) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
static
void priv_box_putAsync( box_t *box, const char *data )
/* -------------------------------------------------------------------------- */
{
	size_t size = box->size;
	size_t i = box->tail;

	while (size--)
		box->data[i++] = *data++;
	box->tail = (i < box->limit) ? i : 0;
	atomic_fetch_add((atomic_uint *)&box->count, box->size);
}

/* -------------------------------------------------------------------------- */
unsigned box_giveAsync( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(box);
	assert(box->data);
	assert(box->limit);
	assert(data);

	sys_lock();
	{
		if (atomic_load((atomic_uint *)&box->count) < box->limit)
		{
			priv_box_putAsync(box, data);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void box_sendAsync( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_giveAsync(box, data) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */

#endif//OS_ATOMICS
