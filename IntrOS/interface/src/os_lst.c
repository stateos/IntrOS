/******************************************************************************

    @file    IntrOS: os_lst.c
    @author  Rajmund Szymanski
    @date    11.01.2017
    @brief   This file provides set of functions for IntrOS.

 ******************************************************************************

    IntrOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of IntrOS distribution.

    IntrOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    IntrOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <os.h>

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
	while (lst_take(lst, data) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
void lst_give( lst_t *lst, void *data )
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
