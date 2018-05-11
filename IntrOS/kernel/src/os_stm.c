/******************************************************************************

    @file    IntrOS: os_stm.c
    @author  Rajmund Szymanski
    @date    11.05.2018
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
char priv_stm_getc( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	unsigned i = stm->first;
	char c = stm->data[i++];
	stm->first = (i < stm->limit) ? i : 0;
	stm->count--;
	return c;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_putc( stm_t *stm, char c )
/* -------------------------------------------------------------------------- */
{
	unsigned i = stm->next;
	stm->data[i++] = c;
	stm->next = (i < stm->limit) ? i : 0;
	stm->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_get( stm_t *stm, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size <= stm->limit);

	while (size--)
		*data++ = priv_stm_getc(stm);
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_put( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size <= stm->limit);

	while (size--)
		priv_stm_putc(stm, *data++);
}

/* -------------------------------------------------------------------------- */
unsigned stm_take( stm_t *stm, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	port_sys_lock();

	if (size <= stm_count(stm))
		priv_stm_get(stm, data, len = size);

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_wait( stm_t *stm, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	port_sys_lock();

	if (size > 0 && size <= stm->limit)
		while ((len = stm_take(stm, data, size)) == 0)
			core_ctx_switch();

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_give( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	port_sys_lock();

	if (size <= stm_space(stm))
		priv_stm_put(stm, data, len = size);

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_send( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	port_sys_lock();

	if (size > 0 && size <= stm->limit)
		while ((len = stm_give(stm, data, size)) == 0)
			core_ctx_switch();

	port_sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
