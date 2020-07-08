/******************************************************************************

    @file    StateOS: osrwlock.c
    @author  Rajmund Szymanski
    @date    08.07.2020
    @brief   This file provides set of functions for StateOS.

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

#include "inc/osrwlock.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void rwl_init( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	assert(rwl);

	sys_lock();
	{
		memset(rwl, 0, sizeof(rwl_t));
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned rwl_takeRead( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(rwl);

	sys_lock();
	{
		if (!rwl->write && rwl->count < RDR_LIMIT)
		{
			rwl->count++;
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void rwl_waitRead( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	while (rwl_takeRead(rwl) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void rwl_giveRead( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	assert(rwl);
	assert(rwl->write == false);
	assert(rwl->count > 0);

	sys_lock();
	{
		rwl->count--;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned rwl_takeWrite( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(rwl);

	sys_lock();
	{
		if (!rwl->write && rwl->count == 0)
		{
			rwl->write = true;
			return SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void rwl_waitWrite( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	while (rwl_takeWrite(rwl) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void rwl_giveWrite( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	assert(rwl);
	assert(rwl->write == true);
	assert(rwl->count == 0);

	sys_lock();
	{
		rwl->write = false;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
