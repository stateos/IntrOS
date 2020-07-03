/******************************************************************************

    @file    IntrOS: osmutex.c
    @author  Rajmund Szymanski
    @date    03.07.2020
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

#include "inc/osmutex.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void mtx_init( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	assert(mtx);

	sys_lock();
	{
		memset(mtx, 0, sizeof(mtx_t));
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned mtx_take( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(mtx);
	assert(mtx->owner != System.cur);

	sys_lock();
	{
		if (mtx->owner == NULL)
		{
			mtx->owner = System.cur;
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void mtx_wait( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	while (mtx_take(mtx) != SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned mtx_give( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned result = FAILURE;

	assert(mtx);

	sys_lock();
	{
		if (mtx->owner == System.cur)
		{
		    mtx->owner = NULL;
			result = SUCCESS;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
