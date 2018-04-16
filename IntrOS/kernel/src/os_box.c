/******************************************************************************

    @file    IntrOS: os_box.c
    @author  Rajmund Szymanski
    @date    16.04.2018
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

#include "inc/os_box.h"

/* -------------------------------------------------------------------------- */
void box_init( box_t *box, unsigned limit, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(box);
	assert(limit);
	assert(data);
	assert(size);

	port_sys_lock();

	memset(box, 0, sizeof(box_t));
	
	box->limit = limit * size;
	box->data  = data;
	box->size  = size;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_box_get( box_t *box, char *data )
/* -------------------------------------------------------------------------- */
{
	unsigned f = box->first;
	unsigned i = 0;

	do data[i++] = box->data[f++]; while (i < box->size);

	box->first = (f < box->limit) ? f : 0;
	box->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_put( box_t *box, const char *data )
/* -------------------------------------------------------------------------- */
{
	unsigned n = box->next;
	unsigned i = 0;

	do box->data[n++] = data[i++]; while (i < box->size);

	box->next = (n < box->limit) ? n : 0;
	box->count++;
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

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void box_wait( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_take(box, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned box_give( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count < box->limit)
	{
		priv_box_put(box, data);

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void box_send( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_give(box, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void box_push( box_t *box, const void *data )
/* -------------------------------------------------------------------------- */
{
	assert(box);
	assert(data);

	port_sys_lock();

	priv_box_put(box, data);

	if (box->count > box->limit)
	{
		box->count = box->limit;
		box->first = box->next;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
