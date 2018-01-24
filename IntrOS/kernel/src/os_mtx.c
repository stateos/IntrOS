/******************************************************************************

    @file    IntrOS: os_mtx.c
    @author  Rajmund Szymanski
    @date    24.01.2018
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

#include "inc/os_mtx.h"

/* -------------------------------------------------------------------------- */
void mtx_init( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	assert(mtx);

	port_sys_lock();

	memset(mtx, 0, sizeof(mtx_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned mtx_take( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(mtx);
	assert(mtx->owner != System.cur);

	port_sys_lock();

	if (mtx->owner == 0)
	{
		mtx->owner = System.cur;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void mtx_wait( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	while (mtx_take(mtx) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned mtx_give( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;
	
	assert(mtx);

	port_sys_lock();

	if (mtx->owner == System.cur)
	{
	    mtx->owner = 0;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
