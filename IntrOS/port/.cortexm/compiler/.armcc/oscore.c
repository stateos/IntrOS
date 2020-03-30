/******************************************************************************

    @file    IntrOS: oscore.c
    @author  Rajmund Szymanski
    @date    29.03.2020
    @brief   IntrOS port file for ARM Cotrex-M uC.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#include "oskernel.h"

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
