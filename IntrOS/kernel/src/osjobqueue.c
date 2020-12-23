/******************************************************************************

    @file    IntrOS: osjobqueue.c
    @author  Rajmund Szymanski
    @date    23.12.2020
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
#if OS_ATOMICS
	atomic_fetch_sub(&job->count, 1);
#else
	job->count--;
#endif
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
#if OS_ATOMICS
	atomic_fetch_add(&job->count, 1);
#else
	job->count++;
#endif
}

/* -------------------------------------------------------------------------- */
static
void priv_job_skip( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned i = job->head + 1;

	job->head = (i < job->limit) ? i : 0;
#if OS_ATOMICS
	atomic_fetch_sub(&job->count, 1);
#else
	job->count--;
#endif
}

/* -------------------------------------------------------------------------- */
unsigned job_take( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t *fun;
	unsigned result = FAILURE;

	assert(job);
	assert(job->data);
	assert(job->limit);

	sys_lock();
	{
#if OS_ATOMICS
		if (atomic_load(&job->count) > 0)
#else
		if (job->count > 0)
#endif
		{
			fun = priv_job_get(job);
			result = SUCCESS;
		}
	}
	sys_unlock();

	if (result == SUCCESS)
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
#if OS_ATOMICS
		if (atomic_load(&job->count) < job->limit)
#else
		if (job->count < job->limit)
#endif
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
#if OS_ATOMICS
		count = atomic_load(&job->count);
#else
		count = job->count;
#endif
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
#if OS_ATOMICS
		space = job->limit - atomic_load(&job->count);
#else
		space = job->limit - job->count;
#endif
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
