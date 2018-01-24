/******************************************************************************

    @file    IntrOS: os_msg.c
    @author  Rajmund Szymanski
    @date    24.01.2018
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
void msg_init( msg_t *msg, unsigned limit, unsigned *data )
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
void priv_msg_get( msg_t *msg, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	*data = msg->data[msg->first];

	msg->first = (msg->first + 1) % msg->limit;
	msg->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, unsigned data )
/* -------------------------------------------------------------------------- */
{
	msg->data[msg->next] = data;

	msg->next = (msg->next + 1) % msg->limit;
	msg->count++;
}

/* -------------------------------------------------------------------------- */
unsigned msg_take( msg_t *msg, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(msg);
	assert(data);

	port_sys_lock();

	if (msg->count > 0)
	{
		priv_msg_get(msg, data);

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void msg_wait( msg_t *msg, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	while (msg_take(msg, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned msg_give( msg_t *msg, unsigned data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(msg);
	assert(data);

	port_sys_lock();

	if (msg->count < msg->limit)
	{
		priv_msg_put(msg, data);

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void msg_send( msg_t *msg, unsigned data )
/* -------------------------------------------------------------------------- */
{
	while (msg_give(msg, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
