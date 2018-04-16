/******************************************************************************

    @file    IntrOS: os_stm.c
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

#include "inc/os_stm.h"

/* -------------------------------------------------------------------------- */
void stm_init( stm_t *stm, unsigned limit, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(stm);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(stm, 0, sizeof(stm_t));

	stm->limit = limit;
	stm->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_get( stm_t *stm, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = 0;

	if (size > stm->count)
		size = stm->count;

	stm->count -= size;

	while (i < size)
	{
		data[i++] = stm->data[stm->first++];
		if (stm->first >= stm->limit)
			stm->first = 0;
	}

	return size;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_put( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = 0;

	if (size > stm->limit - stm->count)
		size = stm->limit - stm->count;

	stm->count += size;

	while (i < size)
	{
		stm->data[stm->next++] = data[i++];
		if (stm->next >= stm->limit)
			stm->next = 0;
	}

	return size;
}

/* -------------------------------------------------------------------------- */
unsigned stm_take( stm_t *stm, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	port_sys_lock();

	if (stm->rdr == 0)
		len = priv_stm_get(stm, data, size);

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
void stm_wait( stm_t *stm, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(stm);
	assert(data);

	port_sys_lock();

	while (stm->rdr) core_ctx_switch();
	stm->rdr = true;
	
	for (;;)
	{
		len = priv_stm_get(stm, data, size);
		data = (char *)data + len;
		size -= len;

		if (size == 0) break;

		core_ctx_switch();
	}

	stm->rdr = false;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned stm_give( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	port_sys_lock();

	if (stm->rdr == 0)
		len = priv_stm_put(stm, data, size);

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
void stm_send( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len;

	assert(stm);
	assert(data);

	port_sys_lock();

	while (stm->wtr) core_ctx_switch();
	stm->wtr = true;
	
	for (;;)
	{
		len = priv_stm_put(stm, data, size);
		data = (const char *)data + len;
		size -= len;

		if (size == 0) break;

		core_ctx_switch();
	}

	stm->wtr = false;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
