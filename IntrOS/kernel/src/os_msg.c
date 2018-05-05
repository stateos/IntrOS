/******************************************************************************

    @file    IntrOS: os_msg.c
    @author  Rajmund Szymanski
    @date    05.05.2018
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

#include "inc/os_msg.h"

/* -------------------------------------------------------------------------- */
void msg_init( msg_t *msg, unsigned limit, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(msg);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(msg, 0, sizeof(msg_t));

	msg->limit = limit;
	msg->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
char priv_msg_getc( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	unsigned i = msg->first;
	char c = msg->data[i++];
	msg->first = (i < msg->limit) ? i : 0;
	msg->count--;
	return c;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_putc( msg_t *msg, char c )
/* -------------------------------------------------------------------------- */
{
	unsigned i = msg->next;
	msg->data[i++] = c;
	msg->next = (i < msg->limit) ? i : 0;
	msg->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_get( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size <= msg->limit);

	while (size--)
		*data++ = priv_msg_getc(msg);
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size <= msg->limit);

	while (size--)
		priv_msg_putc(msg, *data++);
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
unsigned msg_take( msg_t *msg, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	port_sys_lock();

	if (size > 0)
	{
		if (msg->size > 0)
		{
			if (size >= msg_count(msg))
			{
				priv_msg_get(msg, data, len = msg->size);
				priv_msg_getSize(msg);
			}
		}
	}

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_wait( msg_t *msg, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	port_sys_lock();

	if (size > 0)
		while ((len = msg_take(msg, data, size)) == 0)
			core_ctx_switch();

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_give( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	port_sys_lock();

	if (size > 0 && size <= msg->limit)
	{
		if (size <= msg_space(msg))
		{
			priv_msg_putSize(msg, len = size);
			priv_msg_put(msg, data, len);
		}
	}

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_send( msg_t *msg, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	port_sys_lock();

	if (size > 0 && size <= msg->limit)
		while ((len = msg_give(msg, data, size)) == 0)
			core_ctx_switch();

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned msg_count( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	return msg->size;
}

/* -------------------------------------------------------------------------- */
unsigned msg_space( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	if (msg->count == 0)
		return msg->limit;
	else
	if (msg->count + sizeof(unsigned) < msg->limit)
		return msg->limit - msg->count - sizeof(unsigned);
	else
		return 0;
}

/* -------------------------------------------------------------------------- */
