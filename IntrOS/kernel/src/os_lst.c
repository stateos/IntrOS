/******************************************************************************

    @file    IntrOS: os_lst.c
    @author  Rajmund Szymanski
    @date    13.04.2018
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

#include "inc/os_lst.h"

/* -------------------------------------------------------------------------- */
void lst_init( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	assert(lst);

	port_sys_lock();

	memset(lst, 0, sizeof(lst_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned lst_take( lst_t *lst, void **data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(lst);
	assert(data);

	port_sys_lock();

	if (lst->next)
	{
		*data = lst->next + 1;
		lst->next = lst->next->next;
		event = E_SUCCESS;
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void lst_wait( lst_t *lst, void **data )
/* -------------------------------------------------------------------------- */
{
	while (lst_take(lst, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void lst_give( lst_t *lst, const void *data )
/* -------------------------------------------------------------------------- */
{
	que_t *ptr;

	assert(lst);
	assert(data);

	port_sys_lock();

	ptr = (que_t *)&(lst->next);
	while (ptr->next) ptr = ptr->next;
	ptr->next = (que_t *)data - 1;
	ptr->next->next = 0;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
