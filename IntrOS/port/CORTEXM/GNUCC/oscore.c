/******************************************************************************

    @file    IntrOS: oscore.c
    @author  Rajmund Szymanski
    @date    13.07.2017
    @brief   IntrOS port file for ARM Cotrex-M uC.

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

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

#if __FPU_USED

__attribute__((naked))
int setjmp( jmp_buf buf )
{
	__asm volatile
	(
"	mov    ip, sp                \n"
"	stmia  r0!, { r4-r11,ip,lr } \n"
"	vstmia r0!, { s16-s31 }      \n"
"	movs   r0,  # 0              \n"
"	bx     lr                    \n"
::	"r" (buf)
	);
}

#endif

/* -------------------------------------------------------------------------- */

#if __FPU_USED

__attribute__((naked))
void longjmp( jmp_buf buf, int val )
{
	__asm volatile
	(
"	ldmia  r0!, { r4-r11,ip,lr } \n"
"	vldmia r0!, { s16-s31 }      \n"
"	mov    sp, ip                \n"
"	movs   r0, r1                \n"
"	it     eq                    \n"
"	moveq  r0,  # 1              \n"
"	bx     lr                    \n"
::	"r" (buf), "r" (val)
	);
}

#endif

/* -------------------------------------------------------------------------- */

#endif // __GNUC__ && !__ARMCC_VERSION
