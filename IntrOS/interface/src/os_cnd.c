/******************************************************************************

    @file    IntrOS: os_cnd.c
    @author  Rajmund Szymanski
    @date    20.03.2017
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
void cnd_wait( cnd_t *cnd, mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned signal;

	assert(cnd);
	assert(mtx);

	mtx_give(mtx);

	signal = cnd->signal;
	while (cnd->signal == signal) tsk_yield();

	mtx_wait(mtx);
}

/* -------------------------------------------------------------------------- */
void cnd_give( cnd_t *cnd )
/* -------------------------------------------------------------------------- */
{
	assert(cnd);

	port_sys_lock();

	cnd->signal++;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
