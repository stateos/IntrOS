/******************************************************************************

    @file    IntrOS: ossemaphore.c
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
unsigned sem_take( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(sem);
	assert(sem->limit);
	assert(sem->count<=sem->limit);

	sys_lock();
	{
		if (sem->count > 0U)
		{
			sem->count--;
			result = SUCCESS;
		}
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
unsigned sem_give( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(sem);
	assert(sem->limit);
	assert(sem->count<=sem->limit);

	sys_lock();
	{
		if (sem->count < sem->limit)
		{
			sem->count++;
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
unsigned sem_getValue( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned val;

	sys_lock();
	{
		val = sem->count;
	}
	sys_unlock();

	return val;
}

/* -------------------------------------------------------------------------- */
