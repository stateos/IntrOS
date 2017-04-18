/******************************************************************************

    @file    IntrOS: os_box.c
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
void priv_box_get( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char*buf = box->data + box->size * box->first;

	for (i = 0; i < box->size; i++) ((char*)data)[i] = buf[i];

	box->first = (box->first + 1) % box->limit;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_put( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char*buf = box->data + box->size * box->next;

	for (i = 0; i < box->size; i++) buf[i] = ((char*)data)[i];

	box->next = (box->next + 1) % box->limit;
}

/* -------------------------------------------------------------------------- */
unsigned box_take( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count > 0)
	{
		priv_box_get(box, data);

		box->count--;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void box_wait( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_take(box, data) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
unsigned box_give( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count < box->limit)
	{
		priv_box_put(box, data);

		box->count++;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void box_send( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_give(box, data) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
