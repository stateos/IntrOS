/******************************************************************************

    @file    IntrOS: os_mem.c
    @author  Rajmund Szymanski
    @date    27.02.2017
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
void mem_bind( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	void   **ptr;
	unsigned cnt;

	assert(mem);
	assert(mem->data);
	assert(mem->limit);

	port_sys_lock();

	ptr = mem->data;
	cnt = mem->limit;

	mem->next = 0;
	while (cnt--) { mem_give(mem, ++ptr); ptr += mem->size; }

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned mem_take( mem_t *mem, void **data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(mem);
	assert(data);

	port_sys_lock();

	if (mem->next)
	{
		*data = mem->next + 1;
		mem->next = mem->next->next;
		event = E_SUCCESS;
	}
	
	if (event == E_SUCCESS)
	{
		void   **ptr = *data;
		unsigned cnt = mem->size;
		while (cnt--) *ptr++ = 0;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void mem_wait( mem_t *mem, void **data )
/* -------------------------------------------------------------------------- */
{
	while (mem_take(mem, data) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
void mem_give( mem_t *mem, void *data )
/* -------------------------------------------------------------------------- */
{
	que_t *ptr;
	
	assert(mem);
	assert(data);

	port_sys_lock();

	ptr = (que_t *)&(mem->next);
	while (ptr->next) ptr = ptr->next;
	ptr->next = (que_t *)data - 1;
	ptr->next->next = 0;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
