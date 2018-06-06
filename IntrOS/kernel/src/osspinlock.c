/******************************************************************************

    @file    IntrOS: osspinlock.c
    @author  Rajmund Szymanski
    @date    05.06.2018
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

#include "inc/osspinlock.h"

/* -------------------------------------------------------------------------- */
void spn_init( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	assert(spn);

	port_sys_lock();

	memset(spn, 0, sizeof(spn_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned spn_take( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(spn);

	port_sys_lock();

	if (spn->flag == 0)
	{
		spn->flag = 1;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void spn_wait( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	while (spn_take(spn) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void spn_give( spn_t *spn )
/* -------------------------------------------------------------------------- */
{
	assert(spn);

    spn->flag = 0;
}

/* -------------------------------------------------------------------------- */
