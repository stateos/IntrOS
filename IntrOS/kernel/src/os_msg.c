/******************************************************************************

    @file    IntrOS: os_msg.c
    @author  Rajmund Szymanski
    @date    01.05.2018
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

	msg->limit = PSIZE(limit);
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
unsigned priv_msg_get( msg_t *msg, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	if (size > msg->count)
		size = msg->count;

	while (size--)
		data[len++] = priv_msg_getc(msg);

	while (msg->first % sizeof(unsigned))
		priv_msg_getc(msg);

	return len;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_msg_put( msg_t *msg, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	if (size > msg->limit - msg->count)
		size = msg->limit - msg->count;

	while (size--)
		priv_msg_putc(msg, data[len++]);

	while (msg->next % sizeof(unsigned))
		priv_msg_putc(msg, 0);

	return len;
}

/* -------------------------------------------------------------------------- */
static
void priv_put_size( msg_t *msg, unsigned size )
/* -------------------------------------------------------------------------- */
{
	if (msg->size == 0)
		msg->size = size;
	else
		priv_msg_put(msg, (void *)&size, sizeof(unsigned));
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_update( msg_t *msg )
/* -------------------------------------------------------------------------- */
{
	while (msg->count > 0 && msg->size == 0)
		priv_msg_get(msg, (void *)&msg->size, sizeof(unsigned));
}

/* -------------------------------------------------------------------------- */
unsigned msg_take( msg_t *msg, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(msg);
	assert(data);

	port_sys_lock();

	if (msg->rdr == false && msg->size > 0 && msg->size <= size && msg->size <= msg->count)
	{
			size = msg->size; msg->size = 0;
			len = priv_msg_get(msg, data, size);
			priv_msg_update(msg);
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

	while (msg->rdr || msg->count == 0)
		core_ctx_switch();
	msg->rdr = true;
	
	if (msg->size > 0 && msg->size <= size)
	{
		size = msg->size; msg->size = 0;

		for (;;)
		{
			len += priv_msg_get(msg, (char *)data + len, size - len);
			if (len == size) break;
			core_ctx_switch();
		}

		priv_msg_update(msg);
	}

	msg->rdr = false;

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

	if (msg->wtr == false &&
	  ((msg->size == 0 && msg->count + size <= msg->limit) || msg->count + sizeof(unsigned) + size <= msg->limit))
	{
		priv_put_size(msg, size);
		len = priv_msg_put(msg, data, size);
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

	while (msg->wtr || msg->count == msg->limit)
		core_ctx_switch();
	msg->wtr = true;
	
	priv_put_size(msg, size);

	for (;;)
	{
		len += priv_msg_put(msg, (const char *)data + len, size - len);
		if (len == size) break;
		core_ctx_switch();
	}

	msg->wtr = false;

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
