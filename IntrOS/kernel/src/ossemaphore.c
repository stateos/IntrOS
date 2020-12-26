/******************************************************************************

    @file    IntrOS: ossemaphore.c
    @author  Rajmund Szymanski
    @date    26.12.2020
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

#include "inc/ossemaphore.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void sem_init( sem_t *sem, unsigned init, unsigned limit )
/* -------------------------------------------------------------------------- */
{
	assert(sem);
	assert(limit);
	assert(init<=limit);

	sys_lock();
	{
		memset(sem, 0, sizeof(sem_t));

		sem->count = init < limit ? init : limit;
		sem->limit = limit;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sem_take( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	if (sem->count == 0)
		return FAILURE;

	sem->count--;
	return SUCCESS;
}

/* -------------------------------------------------------------------------- */
unsigned sem_take( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned result;

	assert(sem);
	assert(sem->limit);
	assert(sem->count<=sem->limit);

	sys_lock();
	{
		result = priv_sem_take(sem);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void sem_wait( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	while (sem_take(sem) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sem_give( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	if (sem->count >= sem->limit)
		return FAILURE;

	sem->count++;
	return SUCCESS;
}

/* -------------------------------------------------------------------------- */
unsigned sem_give( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned result;

	assert(sem);
	assert(sem->limit);
	assert(sem->count<=sem->limit);

	sys_lock();
	{
		result = priv_sem_give(sem);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned sem_getValue( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned val;

	assert(sem);

	sys_lock();
	{
		val = sem->count;
	}
	sys_unlock();

	return val;
}

/* -------------------------------------------------------------------------- */

#if OS_ATOMICS

/* -------------------------------------------------------------------------- */
static
unsigned priv_sem_takeAsync( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned count = atomic_load((atomic_uint *)&sem->count);

	while (count > 0)
		if (atomic_compare_exchange_weak((atomic_uint *)&sem->count, &count, count - 1))
			return SUCCESS;

	return FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned sem_takeAsync( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned result;

	assert(sem);
	assert(sem->limit);
	assert(sem->count<=sem->limit);

	sys_lock();
	{
		result = priv_sem_takeAsync(sem);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void sem_waitAsync( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	while (sem_takeAsync(sem) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sem_giveAsync( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned count = atomic_load((atomic_uint *)&sem->count);

	while (count < sem->limit)
		if (atomic_compare_exchange_weak((atomic_uint *)&sem->count, &count, count + 1))
			return SUCCESS;

	return FAILURE;
}

/* -------------------------------------------------------------------------- */
unsigned sem_giveAsync( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned result;

	assert(sem);
	assert(sem->limit);
	assert(sem->count<=sem->limit);

	sys_lock();
	{
		result = priv_sem_giveAsync(sem);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */

#endif//OS_ATOMICS
