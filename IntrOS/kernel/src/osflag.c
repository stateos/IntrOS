/******************************************************************************

    @file    IntrOS: osflag.c
    @author  Rajmund Szymanski
    @date    03.08.2018
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

#include "inc/osflag.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void flg_init( flg_t *flg, unsigned init )
/* -------------------------------------------------------------------------- */
{
	assert(flg);

	sys_lock();
	{
		memset(flg, 0, sizeof(flg_t));

		flg->flags = init;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned flg_take( flg_t *flg, unsigned flags, bool all )
/* -------------------------------------------------------------------------- */
{
	unsigned event = flags;

	assert(flg);

	sys_lock();
	{
		if (flags & flg->flags)
		{
			event &= all ? ~flg->flags : 0;
			flg->flags &= ~flags;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void flg_wait( flg_t *flg, unsigned flags, bool all )
/* -------------------------------------------------------------------------- */
{
	while ((flags = flg_take(flg, flags, all)) != 0) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void flg_give( flg_t *flg, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	assert(flg);

	sys_lock();
	{
		flg->flags |= flags;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
