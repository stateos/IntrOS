/******************************************************************************

    @file    IntrOS: osjobqueue.c
    @author  Rajmund Szymanski
    @date    27.12.2020
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

#include "inc/osjobqueue.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void job_init( job_t *job, fun_t **data, size_t bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(job, 0, sizeof(job_t));

		job->limit = bufsize / sizeof(fun_t *);
		job->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
fun_t *priv_job_get( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned i = job->head;

	fun_t *fun = job->data[i++];

	job->head = (i < job->limit) ? i : 0;
	job->count--;

	return fun;
}

/* -------------------------------------------------------------------------- */
static
void priv_job_put( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	unsigned i = job->tail;

	job->data[i++] = fun;

	job->tail = (i < job->limit) ? i : 0;
	job->count++;
}

/* -------------------------------------------------------------------------- */
static
void priv_job_skip( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned i = job->head + 1;

	job->head = (i < job->limit) ? i : 0;
	job->count--;
}

/* -------------------------------------------------------------------------- */
unsigned job_take( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t *fun = NULL;
	unsigned result = FAILURE;

	assert(job);
	assert(job->data);
	assert(job->limit);

	sys_lock();
	{
		if (job->count > 0)
		{
			fun = priv_job_get(job);
			result = SUCCESS;
		}
	}
	sys_unlock();

	if (fun != NULL)
		fun();

	return result;
}

/* -------------------------------------------------------------------------- */
void job_wait( job_t *job )
/* -------------------------------------------------------------------------- */
{
	while (job_take(job) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned job_give( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(job);
	assert(job->data);
	assert(job->limit);
	assert(fun);

	sys_lock();
	{
		if (job->count < job->limit)
		{
			priv_job_put(job, fun);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void job_send( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	while (job_give(job, fun) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void job_push( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(job->data);
	assert(job->limit);
	assert(fun);

	sys_lock();
	{
		if (job->count == job->limit)
			priv_job_skip(job);
		priv_job_put(job, fun);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned job_count( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned count;

	assert(job);

	sys_lock();
	{
		count = job->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
unsigned job_space( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned space;

	assert(job);

	sys_lock();
	{
		space = job->limit - job->count;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
unsigned job_limit( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned limit;

	assert(job);

	sys_lock();
	{
		limit = job->limit;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */

#if OS_ATOMICS

/* -------------------------------------------------------------------------- */
static
fun_t *priv_job_getAsync( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned i = job->head;

	fun_t *fun = job->data[i++];

	job->head = (i < job->limit) ? i : 0;
	atomic_fetch_sub((atomic_uint *)&job->count, 1);

	return fun;
}

/* -------------------------------------------------------------------------- */
unsigned job_takeAsync( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t *fun = NULL;
	unsigned result = FAILURE;

	assert(job);
	assert(job->data);
	assert(job->limit);

	sys_lock();
	{
		if (atomic_load((atomic_uint *)&job->count) > 0)
		{
			fun = priv_job_getAsync(job);
			result = SUCCESS;
		}
	}
	sys_unlock();

	if (fun != NULL)
		fun();

	return result;
}

/* -------------------------------------------------------------------------- */
void job_waitAsync( job_t *job )
/* -------------------------------------------------------------------------- */
{
	while (job_takeAsync(job) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
static
void priv_job_putAsync( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	unsigned i = job->tail;

	job->data[i++] = fun;

	job->tail = (i < job->limit) ? i : 0;
	atomic_fetch_add((atomic_uint *)&job->count, 1);
}

/* -------------------------------------------------------------------------- */
unsigned job_giveAsync( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(job);
	assert(job->data);
	assert(job->limit);
	assert(fun);

	sys_lock();
	{
		if (atomic_load((atomic_uint *)&job->count) < job->limit)
		{
			priv_job_putAsync(job, fun);
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void job_sendAsync( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	while (job_giveAsync(job, fun) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */

#endif//OS_ATOMICS
