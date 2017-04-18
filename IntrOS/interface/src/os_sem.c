/******************************************************************************

    @file    IntrOS: os_sem.c
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
unsigned sem_take( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(sem);

	port_sys_lock();

	if (sem->count > 0)
	{
		sem->count--;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void sem_wait( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	while (sem_take(sem) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
unsigned sem_give( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(sem);

	port_sys_lock();

	if (sem->count < ~0U)
	{
		sem->count++;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void sem_send( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	while (sem_give(sem) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
