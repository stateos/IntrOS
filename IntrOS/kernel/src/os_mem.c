/******************************************************************************

    @file    IntrOS: os_mem.c
    @author  Rajmund Szymanski
    @date    12.12.2017
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

#include "inc/os_mem.h"

/* -------------------------------------------------------------------------- */
void mem_bind( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	que_t  * ptr;
	unsigned cnt;

	assert(mem);
	assert(mem->limit);
	assert(mem->size);
	assert(mem->data);

	port_sys_lock();
	
	ptr = mem->data;
	cnt = mem->limit;

	mem->next = 0;
	while (cnt--) { mem_give(mem, ++ptr); ptr += mem->size; }

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mem_init( mem_t *mem, unsigned limit, unsigned size, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(mem);
	assert(limit);
	assert(size);
	assert(data);

	port_sys_lock();

	memset(mem, 0, sizeof(mem_t));

	mem->limit = limit;
	mem->size  = size;
	mem->data  = data;

	mem_bind(mem);

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
		memset(*data, 0, mem->size * sizeof(que_t));

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void mem_wait( mem_t *mem, void **data )
/* -------------------------------------------------------------------------- */
{
	while (mem_take(mem, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void mem_give( mem_t *mem, void *data )
/* -------------------------------------------------------------------------- */
{
	que_t *ptr;

	assert(mem);
	assert(data);

	port_sys_lock();

	ptr = (que_t *)data - 1;
	ptr->next = mem->next;
	mem->next = ptr;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
