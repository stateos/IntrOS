/******************************************************************************

    @file    IntrOS: os_mem.c
    @author  Rajmund Szymanski
    @date    07.11.2016
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
void mem_init( mem_id mem )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();
	
	void   **ptr = mem->data;
	unsigned cnt = mem->limit;

	mem->next = 0;
	while (cnt--) { mem_give(mem, ptr + 1); ptr += mem->size; }

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned mem_take( mem_id mem, void **data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	port_sys_lock();

	if (mem->next)
	{
		*data = mem->next + 1;
		mem->next = *mem->next;
		event = E_SUCCESS;
	}
	
	if (event == E_SUCCESS)
	{
		void   **ptr = *data;
		unsigned cnt = mem->size;
		while (--cnt) *ptr++ = 0;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void mem_wait( mem_id mem, void **data )
/* -------------------------------------------------------------------------- */
{
	while (mem_take(mem, data) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
void mem_give( mem_id mem, void *data )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	void **ptr = (void**)&mem->next;
	while (*ptr) ptr = *ptr;
	ptr = *ptr = (void**)data - 1; *ptr = 0;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */