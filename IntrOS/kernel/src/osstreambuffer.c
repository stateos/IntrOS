/******************************************************************************

    @file    IntrOS: osstreambuffer.c
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

#include "inc/osstreambuffer.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void stm_init( stm_t *stm, void *data, size_t bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(stm);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(stm, 0, sizeof(stm_t));

		stm->limit = bufsize;
		stm->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_get( stm_t *stm, char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t i = stm->head;

	stm->count -= size;
	while (size--)
	{
		*data++ = stm->data[i++];
		if (i == stm->limit) i = 0;
	}
	stm->head = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_put( stm_t *stm, const char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t i = stm->tail;

	stm->count += size;
	while (size--)
	{
		stm->data[i++] = *data++;
		if (i == stm->limit) i = 0;
	}
	stm->tail = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_skip( stm_t *stm, size_t size )
/* -------------------------------------------------------------------------- */
{
	stm->count -= size;
	stm->head  += size;
	if (stm->head >= stm->limit) stm->head -= stm->limit;
}

/* -------------------------------------------------------------------------- */
static
size_t priv_stm_getUpdate( stm_t *stm, char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	if (size > stm->count) size = stm->count;
	priv_stm_get(stm, data, size);
	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_putUpdate( stm_t *stm, const char *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	priv_stm_put(stm, data, size);
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_skipUpdate( stm_t *stm, size_t size )
/* -------------------------------------------------------------------------- */
{
	if (stm->count + size > stm->limit)
		priv_stm_skip(stm, stm->count + size - stm->limit);
}

/* -------------------------------------------------------------------------- */
size_t stm_take( stm_t *stm, void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t result = 0;

	assert(stm);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		if (stm->count > 0)
			result = priv_stm_getUpdate(stm, data, size);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
size_t stm_wait( stm_t *stm, void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	size_t result;

	while (result = stm_take(stm, data, size), result == 0) core_ctx_switch();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned stm_give( stm_t *stm, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(stm);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		if (stm->count + size <= stm->limit)
		{
			priv_stm_putUpdate(stm, data, size);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned stm_send( stm_t *stm, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	if (size > stm->limit)
		return FAILURE;

	while (stm_give(stm, data, size) != SUCCESS) core_ctx_switch();

	return SUCCESS;
}

/* -------------------------------------------------------------------------- */
unsigned stm_push( stm_t *stm, const void *data, size_t size )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(stm);
	assert(stm->data);
	assert(stm->limit);
	assert(data);
	assert(size);

	sys_lock();
	{
		if (size <= stm->limit)
		{
			priv_stm_skipUpdate(stm, size);
			priv_stm_putUpdate(stm, data, size);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
size_t stm_count( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	size_t count;

	assert(stm);

	sys_lock();
	{
		count = stm->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
size_t stm_space( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	size_t space;

	assert(stm);

	sys_lock();
	{
		space = stm->limit - stm->count;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
size_t stm_limit( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	size_t limit;

	assert(stm);

	sys_lock();
	{
		limit = stm->limit;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */
