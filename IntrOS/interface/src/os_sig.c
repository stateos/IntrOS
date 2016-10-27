/******************************************************************************

    @file    IntrOS: os_sig.c
    @author  Rajmund Szymanski
    @date    27.10.2016
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
unsigned sig_take( sig_id sig )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	port_sys_lock();

	if (sig->flag)
	{
		sig->flag = sig->type;

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void sig_wait( sig_id sig )
/* -------------------------------------------------------------------------- */
{
	while (sig_take(sig) != E_SUCCESS) tsk_yield();
}

/* -------------------------------------------------------------------------- */
void sig_give( sig_id sig )
/* -------------------------------------------------------------------------- */
{
	sig->flag = 1;
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_id sig )
/* -------------------------------------------------------------------------- */
{
	sig->flag = 0;
}

/* -------------------------------------------------------------------------- */
