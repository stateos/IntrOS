/******************************************************************************

    @file    IntrOS: osstreambuffer.c
    @author  Rajmund Szymanski
    @date    27.08.2018
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

#include "inc/osstreambuffer.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void stm_init( stm_t *stm, void *data, unsigned bufsize )
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
unsigned priv_stm_count( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	return stm->count;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_space( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	return stm->limit - stm->count;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_limit( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	return stm->limit;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_get( stm_t *stm, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = stm->head;

	stm->count -= size;
	while (size--)
	{
		*data++ = stm->data[i++];
		if (i >= stm->limit) i = 0;
	}
	stm->head = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_put( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned i = stm->tail;

	stm->count += size;
	while (size--)
	{
		stm->data[i++] = *data++;
		if (i >= stm->limit) i = 0;
	}
	stm->tail = i;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_skip( stm_t *stm, unsigned size )
/* -------------------------------------------------------------------------- */
{
	stm->count -= size;
	stm->head  += size;
	if (stm->head >= stm->limit) stm->head -= stm->limit;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_stm_getUpdate( stm_t *stm, char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	if (size > stm->count)
		size = stm->count;
	priv_stm_get(stm, data, size);

	return size;
}

/* -------------------------------------------------------------------------- */
static
void priv_stm_putUpdate( stm_t *stm, const char *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(size <= priv_stm_space(stm));

	priv_stm_put(stm, data, size);
}

/* -------------------------------------------------------------------------- */
unsigned stm_take( stm_t *stm, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	sys_lock();
	{
		if (stm->count > 0)
		{
			if (size > 0)
				size = priv_stm_getUpdate(stm, data, size);
			len = size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_wait( stm_t *stm, void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	sys_lock();
	{
		if (size > 0)
			while ((len = stm_take(stm, data, size)) == 0)
				core_ctx_switch();
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_give( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	sys_lock();
	{
		if (size <= priv_stm_space(stm))
		{
			if (size > 0)
				priv_stm_putUpdate(stm, data, size);
			len = size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_send( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	sys_lock();
	{
		if (size > 0 && size <= priv_stm_limit(stm))
			while ((len = stm_give(stm, data, size)) == 0)
				core_ctx_switch();
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_push( stm_t *stm, const void *data, unsigned size )
/* -------------------------------------------------------------------------- */
{
	unsigned len = 0;

	assert(stm);
	assert(data);

	sys_lock();
	{
		if (size <= priv_stm_limit(stm))
		{
			if (size > priv_stm_space(stm))
				priv_stm_skip(stm, size - priv_stm_space(stm));
			if (size > 0)
				priv_stm_putUpdate(stm, data, size);
			len = size;
		}
	}
	sys_unlock();

	return len;
}

/* -------------------------------------------------------------------------- */
unsigned stm_count( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(stm);

	sys_lock();
	{
		cnt = priv_stm_count(stm);
	}
	sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned stm_space( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(stm);

	sys_lock();
	{
		cnt = priv_stm_space(stm);
	}
	sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
unsigned stm_limit( stm_t *stm )
/* -------------------------------------------------------------------------- */
{
	unsigned cnt;

	assert(stm);

	sys_lock();
	{
		cnt = priv_stm_limit(stm);
	}
	sys_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */
