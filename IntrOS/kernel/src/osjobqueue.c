/******************************************************************************

    @file    IntrOS: osjobqueue.c
    @author  Rajmund Szymanski
    @date    16.07.2018
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

#include "inc/osjobqueue.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void job_init( job_t *job, unsigned limit, fun_t **data )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(limit);
	assert(data);

	sys_lock();
	{
		memset(job, 0, sizeof(job_t));

		job->limit = limit;
		job->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
fun_t *priv_job_get( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t  * fun;
	unsigned i = job->head;

	fun = job->data[i++];
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
unsigned job_take( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t  * fun;
	unsigned event = E_FAILURE;

	assert(job);

	sys_lock();
	{
		if (job->count > 0)
		{
			fun = priv_job_get(job);

			port_clr_lock();
			fun();

			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void job_wait( job_t *job )
/* -------------------------------------------------------------------------- */
{
	while (job_take(job) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned job_give( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(job);
	assert(fun);

	sys_lock();
	{
		if (job->count < job->limit)
		{
			priv_job_put(job, fun);

			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void job_send( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	while (job_give(job, fun) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void job_push( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(fun);

	sys_lock();
	{
		priv_job_put(job, fun);

		if (job->count > job->limit)
		{
			job->count = job->limit;
			job->head = job->tail;
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
