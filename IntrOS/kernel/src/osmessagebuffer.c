/******************************************************************************

    @file    IntrOS: osmessagebuffer.c
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

#include "inc/osmessagebuffer.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void msg_init( msg_t *msg, void *data, size_t bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(msg);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(msg, 0, sizeof(msg_t));

		msg->limit = bufsize;
		msg->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_peek( msg_t *msg, char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t i = msg->head;

	while (size--)
	{
		*data++ = msg->data[i++];
		if (i == msg->limit) i = 0;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_get( msg_t *msg, char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t i = msg->head;

	msg->count -= size;
	while (size--)
	{
		*data++ = msg->data[i++];
		if (i == msg->limit) i = 0;
	}
	msg->head = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, const char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t i = msg->tail;

	msg->count += size;
	while (size--)
	{
		msg->data[i++] = *data++;
		if (i == msg->limit) i = 0;
	}
	msg->tail = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_skip( msg_t *msg, size_t size )
/* -------------------------------------------------------------------------- */
{
	msg->count -= size;
	msg->head  += size;
	if (msg->head >= msg->limit) msg->head -= msg->limit;
}

/* -------------------------------------------------------------------------- */
static
size_t priv_msg_size( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t size;

	assert(msg->count);

	priv_msg_peek(msg, (void *)&size, sizeof(size_t));

	return size;
}

/* -------------------------------------------------------------------------- */
static
size_t priv_msg_getSize( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t size;

	assert(msg->count);

	priv_msg_get(msg, (void *)&size, sizeof(size_t));

	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putSize( msg_t *msg, size_t size )
/* -------------------------------------------------------------------------- */
{
	priv_msg_put(msg, (const void *)&size, sizeof(size_t));
}

/* -------------------------------------------------------------------------- */
static
size_t priv_msg_getUpdate( msg_t *msg, char *data )
/* -------------------------------------------------------------------------- */
{
	size_t size = priv_msg_getSize(msg);
	priv_msg_get(msg, data, size);
	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putUpdate( msg_t *msg, const char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	priv_msg_putSize(msg, size);
	priv_msg_put(msg, data, size);
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_skipUpdate( msg_t *msg, size_t size )
/* -------------------------------------------------------------------------- */
{
	while (msg->count + sizeof(size_t) + size > msg->limit)
	{
		priv_msg_skip(msg, priv_msg_getSize(msg));
	}
}

/* -------------------------------------------------------------------------- */
size_t msg_take( msg_t *msg, void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t result = 0;

	assert(msg);
	assert(msg->data);
	assert(msg->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		if (msg->count > 0 && size >= priv_msg_size(msg))
			result = priv_msg_getUpdate(msg, data);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
size_t msg_wait( msg_t *msg, void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t result = 0;

	while (result = msg_take(msg, data, size), result == 0) core_ctx_switch();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned msg_give( msg_t *msg, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(msg);
	assert(msg->data);
	assert(msg->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		if (msg->count + sizeof(size_t) + size <= msg->limit)
		{
			priv_msg_putUpdate(msg, data, size);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned msg_send( msg_t *msg, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	if (sizeof(size_t) + size > msg->limit)
		return FAILURE;

	while (msg_give(msg, data, size) != SUCCESS) core_ctx_switch();

	return SUCCESS;
}

/* -------------------------------------------------------------------------- */
unsigned msg_push( msg_t *msg, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(msg);
	assert(msg->data);
	assert(msg->limit);
	assert(data||size==0);

	sys_lock();
	{
		if (sizeof(size_t) + size <= msg->limit)
		{
			priv_msg_skipUpdate(msg, size);
			priv_msg_putUpdate(msg, data, size);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
size_t msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t count;

	assert(msg);

	sys_lock();
	{
		count = msg->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
size_t msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t space;

	assert(msg);

	sys_lock();
	{
		space = (msg->limit >= msg->count + sizeof(size_t)) ? msg->limit - msg->count - sizeof(size_t) : 0;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
size_t msg_limit( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t limit;

	assert(msg);

	sys_lock();
	{
		limit = (msg->limit >= sizeof(size_t)) ? msg->limit - sizeof(size_t) : 0;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */
size_t msg_size( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	size_t size = 0;

	assert(msg);

	sys_lock();
	{
		if (msg->count > 0)
			size = priv_msg_size(msg);
	}
	sys_unlock();

	return size;
}

/* -------------------------------------------------------------------------- */
