/******************************************************************************

    @file    IntrOS: oslist.c
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

#include "inc/oslist.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void lst_init( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	assert(lst);

	sys_lock();
	{
		memset(lst, 0, sizeof(lst_t));
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void *lst_take( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	void *result = NULL;

	assert(lst);

	sys_lock();
	{
		if (lst->head.next)
		{
			result = lst->head.next + 1;
			lst->head.next = lst->head.next->next;
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void *lst_wait( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	void *result;

	while (result = lst_take(lst), result != NULL) core_ctx_switch();

	return result;
}

/* -------------------------------------------------------------------------- */
void lst_give( lst_t *lst, void *data )
/* -------------------------------------------------------------------------- */
{
	que_t *ptr;

	assert(lst);
	assert(data);

	sys_lock();
	{
		for (ptr = &lst->head; ptr->next; ptr = ptr->next);
		ptr->next = (que_t *)data - 1;
		ptr->next->next = 0;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
