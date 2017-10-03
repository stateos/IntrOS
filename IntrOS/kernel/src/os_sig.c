/******************************************************************************

    @file    IntrOS: os_sig.c
    @author  Rajmund Szymanski
    @date    03.10.2017
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

#include "inc/os_sig.h"

/* -------------------------------------------------------------------------- */
void sig_init( sig_t *sig, unsigned type )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	port_sys_lock();

	memset(sig, 0, sizeof(sig_t));

	sig->type = type & sigMASK;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned sig_take( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(sig);

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
void sig_wait( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	while (sig_take(sig) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void sig_give( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	sig->flag = 1;
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_t *sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	sig->flag = 0;
}

/* -------------------------------------------------------------------------- */
