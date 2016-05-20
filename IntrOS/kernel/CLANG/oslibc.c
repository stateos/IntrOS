/******************************************************************************

    @file    IntrOS: oslibc.c
    @author  Rajmund Szymanski
    @date    20.05.2016
    @brief   This file provides set of variables and functions for IntrOS.

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

#if defined(__ARMCOMPILER_VERSION) && !defined(__MICROLIB)

#include <os.h>

/* -------------------------------------------------------------------------- */

__attribute__((weak))
char *_sys_command_string( char *cmd, int len )
{
	(void) len;

	return cmd;
}

/* -------------------------------------------------------------------------- */

#endif // __ARMCOMPILER_VERSION && !__MICROLIB
