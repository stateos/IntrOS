/******************************************************************************

    @file    IntrOS: oscore.c
    @author  Rajmund Szymanski
    @date    07.02.2016
    @brief   IntrOS port file for ARM Cotrex-M4F.

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

#ifdef __CC_ARM

#include <oskernel.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- */

__asm void core_ctx_switch( void )
{
	PRESERVE8
	IMPORT System
	IMPORT core_tsk_handler
	EXPORT core_tsk_break

	mrs   r2,    IPSR
#if __CORTEX_M < 3
	cmp   r2,   #0
	bne   priv_ctx_exit        // inside ISR
#else
	cbnz  r2,    priv_ctx_exit // inside ISR
#endif
	mrs   r3,    PRIMASK
#if __CORTEX_M < 3
	sub   sp,   #40
	mov   r1,    sp
	stm   r1!, { r3  - r7 }
	mov   r3,    r8
	mov   r4,    r9
	mov   r5,    r10
	mov   r6,    r11
	mov   r7,    lr
	stm   r1!, { r3  - r7 }
#else
	push       { r3  - r11, lr }
#if __FPU_USED
	vpush      { s16 - s31 }
#endif
#endif
	mov   r1,    sp

priv_tsk_save
	
	ldr   r0,   =System
	ldr   r0,  [ r0, #__cpp(offsetof(sys_t, cur)) ]
	str   r1,  [ r0, #__cpp(offsetof(tsk_t, sp)) ]
	bl    core_tsk_handler
	ldr   r1,  [ r0, #__cpp(offsetof(tsk_t, sp)) ]
#if __CORTEX_M < 3
	cmp   r1,   #0
	beq   priv_tsk_start
#else
	cbz   r1,    priv_tsk_start
#endif
	mov   sp,    r1
#if __CORTEX_M < 3
	adds  r1,   #20
	ldm   r1!, { r3  - r7 }
	mov   r8,    r3
	mov   r9,    r4
	mov   r10,   r5
	mov   r11,   r6
	mov   lr,    r7
	subs  r1,   #40
	ldm   r1!, { r3  - r7 }
	add   sp,   #40
#else
#if __FPU_USED
	vpop       { s16 - s31 }
#endif
	pop        { r3  - r11, lr }
#endif
	msr   PRIMASK, r3

priv_ctx_exit

	bx    lr

priv_tsk_start

	ldr   r1,  [ r0, #__cpp(offsetof(tsk_t, top)) ]
	mov   sp,    r1
	ldr   r3,  [ r0, #__cpp(offsetof(tsk_t, state)) ]
	blx   r3
	
core_tsk_break

	movs  r1,   #0
	b     priv_tsk_save

	ALIGN
}

/* -------------------------------------------------------------------------- */

#endif // __CC_ARM
