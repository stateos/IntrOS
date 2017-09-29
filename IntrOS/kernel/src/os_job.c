/******************************************************************************

    @file    IntrOS: os_job.c
    @author  Rajmund Szymanski
    @date    29.09.2017
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
void job_init( job_t *job, unsigned limit, fun_t **data )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(job, 0, sizeof(job_t));

	job->limit = limit;
	job->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
fun_t *priv_job_get( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t *proc = job->data[job->first];

	job->first = (job->first + 1) % job->limit;
	job->count--;

	return proc;
}

/* -------------------------------------------------------------------------- */
static
void priv_job_put( job_t *job, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	job->data[job->next] = proc;

	job->next = (job->next + 1) % job->limit;
	job->count++;
}

/* -------------------------------------------------------------------------- */
unsigned job_take( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t  * proc;
	unsigned event = E_FAILURE;

	assert(job);

	port_sys_lock();

	if (job->count > 0)
	{
		proc = priv_job_get(job);

		port_clr_lock();
		proc();

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void job_wait( job_t *job )
/* -------------------------------------------------------------------------- */
{
	while (job_take(job) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
unsigned job_give( job_t *job, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(job);
	assert(proc);

	port_sys_lock();

	if (job->count < job->limit)
	{
		priv_job_put(job, proc);

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void job_send( job_t *job, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	while (job_give(job, proc) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
