/******************************************************************************

    @file    IntrOS: ossignal.c
    @author  Rajmund Szymanski
    @date    14.10.2018
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
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void sig_init( sig_t *sig, unsigned mask )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	sys_lock();
	{
		memset(sig, 0, sizeof(sig_t));

		sig->mask = mask;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned sig_take( sig_t *sig, unsigned sigset )
/* -------------------------------------------------------------------------- */
{
	unsigned signo = E_FAILURE;

	assert(sig);
	assert(sigset);

	sys_lock();
	{
		sigset &= sig->flags;

		if (sigset)
		{
			sigset &= -sigset;
			sig->flags &= ~sigset | sig->mask;
			for (signo = 0; sigset >>= 1; signo++);
		}
	}
	sys_unlock();

	return signo;
}

/* -------------------------------------------------------------------------- */
unsigned sig_wait( sig_t *sig, unsigned sigset )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(sig);
	assert(sigset);

	while ((event = sig_take(sig, sigset)) == E_FAILURE) core_ctx_switch();

	return event;
}

/* -------------------------------------------------------------------------- */
void sig_give( sig_t *sig, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned sigset = SIGSET(signo);

	assert(sig);
	assert(sigset);

	sys_lock();
	{
		sig->flags |= sigset;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_t *sig, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned sigset = SIGSET(signo);

	assert(sig);
	assert(sigset);

	sys_lock();
	{
		sig->flags &= ~sigset;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
