/******************************************************************************

    @file    IntrOS: os_msg.c
    @author  Rajmund Szymanski
    @date    11.01.2017
    @brief   This file provides set of functions for IntrOS.

 ******************************************************************************

    IntrOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of IntrOS distribution.

    IntrOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    IntrOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <os.h>

/* -------------------------------------------------------------------------- */
static
void priv_msg_get( msg_t *msg, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	*data = msg->data[msg->first];

	msg->first = (msg->first + 1) % msg->limit;
}

/* -------------------------------------------------------------------------- */
static
void priv_msg_put( msg_t *msg, unsigned data )
/* -------------------------------------------------------------------------- */
{
	msg->data[msg->next] = data;

	msg->next = (msg->next + 1) % msg->limit;
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

		msg->count--;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void msg_wait( msg_t *msg, unsigned *data )
/* -------------------------------------------------------------------------- */
{
	while (msg_take(msg, data) != E_SUCCESS) tsk_yield();
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

		msg->count++;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void msg_send( msg_t *msg, unsigned data )
/* -------------------------------------------------------------------------- */
{
	while (msg_give(msg, data) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
