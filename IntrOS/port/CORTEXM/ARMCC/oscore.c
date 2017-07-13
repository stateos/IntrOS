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

#if defined(__CC_ARM)

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

#if __FPU_USED

__asm int setjmp( jmp_buf buf )
{
	PRESERVE8

	mov    ip,    sp
	stmia  r0!, { r8-r11,lr }
	stmia  r0!, { r4-r7,ip }
	vstmia r0!, { s16-s31 }
	movs   r0,  # 0
	bx     lr

	ALIGN
}

#endif

/* -------------------------------------------------------------------------- */

#if __FPU_USED

__asm void longjmp( jmp_buf buf, int val )
{
	PRESERVE8

	ldmia  r0!, { r8-r11,lr }
	ldmia  r0!, { r4-r7,ip }
	vldmia r0!, { s16-s31 }
	mov    sp,    ip
	movs   r0,    r1
	it     eq
	moveq  r0,  # 1
	bx     lr

	ALIGN
}

#endif

/* -------------------------------------------------------------------------- */

#endif // __CC_ARM
