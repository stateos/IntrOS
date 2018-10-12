/******************************************************************************

    @file    IntrOS: ossignal.c
    @author  Rajmund Szymanski
    @date    12.10.2018
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
	unsigned flags;
	unsigned signo;

	assert(sig);
	assert(sigset);

	sys_lock();
	{
		flags = sigset & sig->flags;
		signo = sizeof(unsigned) * CHAR_BIT;

		if (flags)
		{
			do signo--; while ((flags <<= 1) != 0);
			sig->flags &= ~SIGSET(signo) | sig->mask;
		}
		else
			signo = E_FAILURE;
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
	unsigned flag = SIGSET(signo);

	assert(sig);
	assert(flag);

	sys_lock();
	{
		sig->flags |= flag;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_t *sig, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned flag = SIGSET(signo);

	assert(sig);
	assert(flag);

	sys_lock();
	{
		sig->flags &= ~flag;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
bool sig_get( sig_t *sig, unsigned signo )
/* -------------------------------------------------------------------------- */
{
	unsigned flag = SIGSET(signo);

	assert(sig);
	assert(flag);

	sys_lock();
	{
		flag &= sig->flags;
	}
	sys_unlock();

	return flag != 0;
}

/* -------------------------------------------------------------------------- */
