/******************************************************************************

    @file    IntrOS: osmessagebuffer.c
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

#include "inc/osmessagebuffer.h"

/* -------------------------------------------------------------------------- */
void msg_init( msg_t *msg, unsigned limit, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(msg);
	assert(limit);
	assert(data);

	core_sys_lock();

	memset(msg, 0, sizeof(msg_t));

	msg->limit = limit;
	msg->data  = data;

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	return msg->size;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	return (msg->count == 0) ?                            msg->limit :
	       (msg->limit - msg->count > sizeof(unsigned)) ? msg->limit - msg->count - sizeof(unsigned) :
	                                                      0;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_skip( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	msg->count -= msg->size;
	msg->head  += msg->size;
	if (msg->head >= msg->limit) msg->head -= msg->limit;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_get( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i;

	msg->count -= size;;
	i = msg->head;
	while (size--)
	{
		*data++ = msg->data[i++];
		if (i >= msg->limit) i = 0;
	}
	msg->head = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i;

	msg->count += size;
	i = msg->tail;
	while (size--)
	{
		msg->data[i++] = *data++;
		if (i >= msg->limit) i = 0;
	}
	msg->tail = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_getSize( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	if (msg->count == 0)
		msg->size = 0;
	else
		priv_msg_get(msg, (void *)&msg->size, sizeof(unsigned));
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putSize( msg_t *msg, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size);

	if (msg->count == 0)
		msg->size = size;
	else
		priv_msg_put(msg, (const void *)&size, sizeof(unsigned));
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_skipUpdate( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	priv_msg_skip(msg);
	priv_msg_getSize(msg);
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_getUpdate( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size >= priv_msg_count(msg));

	priv_msg_get(msg, data, size);
	priv_msg_getSize(msg);
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putUpdate( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size <= priv_msg_space(msg));

	priv_msg_putSize(msg, size);
	priv_msg_put(msg, data, size);
}

/* -------------------------------------------------------------------------- */
unsigned msg_take( msg_t *msg, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	core_sys_lock();

	if (msg->count > 0 && size >= priv_msg_count(msg))
		priv_msg_getUpdate(msg, data, len = msg->size);

	core_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_wait( msg_t *msg, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	core_sys_lock();

	if (size > 0)
	{
		while (msg->count == 0)
			core_ctx_switch();

		len = msg_take(msg, data, size);
	}

	core_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_give( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	core_sys_lock();

	if (size > 0 && size <= priv_msg_space(msg))
		priv_msg_putUpdate(msg, data, len = size);

	core_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_send( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	core_sys_lock();

	if (size > 0 && size <= msg->limit)
	{
		while (size > priv_msg_space(msg))
			core_ctx_switch();

		len = msg_give(msg, data, size);
	}

	core_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_push( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	core_sys_lock();

	if (size > 0 && size <= msg->limit)
	{
		while (size > priv_msg_space(msg))
			priv_msg_skipUpdate(msg);
		priv_msg_putUpdate(msg, data, len = size);
	}

	core_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(msg);

	core_sys_lock();

	cnt = priv_msg_count(msg);

	core_sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt = 0;

	assert(msg);

	core_sys_lock();

	cnt = priv_msg_space(msg);

	core_sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
