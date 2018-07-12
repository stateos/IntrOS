/******************************************************************************

    @file    IntrOS: ossignal.c
    @author  Rajmund Szymanski
    @date    11.07.2018
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

#include "inc/ossignal.h"

/* -------------------------------------------------------------------------- */
void sig_init( sig_t *sig, unsigned type )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	core_sys_lock();

	memset(sig, 0, sizeof(sig_t));

	sig->type = type & sigMASK;

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned sig_take( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(sig);

	core_sys_lock();

	if (sig->flag)
	{
		sig->flag = sig->type;

		event = E_SUCCESS;
	}

	core_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void sig_wait( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	while (sig_take(sig) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void sig_give( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	sig->flag = 1;
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	sig->flag = 0;
}

/* -------------------------------------------------------------------------- */
