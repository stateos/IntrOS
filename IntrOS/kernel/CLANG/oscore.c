/******************************************************************************

    @file    IntrOS: oscore.c
    @author  Rajmund Szymanski
    @date    20.05.2016
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

#if defined(__ARMCOMPILER_VERSION)

#include <stddef.h>
#include <os.h>

/* -------------------------------------------------------------------------- */

__attribute__((naked))
void core_ctx_switch( void )
{
	__asm volatile
	(
"	.global      core_tsk_break    \n"

"	mrs   r2,    IPSR              \n"
#if __CORTEX_M < 3
"	cmp   r2,   #0                 \n"
"	bne   priv_ctx_exit            \n"
#else
"	cbnz  r2,    priv_ctx_exit     \n"
#endif
"	mrs   r3,    PRIMASK           \n"
#if __CORTEX_M < 3
"	sub   sp,   #40                \n"
"	mov   r1,    sp                \n"
"	stm   r1!, { r3  - r7 }        \n"
"	mov   r3,    r8                \n"
"	mov   r4,    r9                \n"
"	mov   r5,    r10               \n"
"	mov   r6,    r11               \n"
"	mov   r7,    lr                \n"
"	stm   r1!, { r3  - r7 }        \n"
#else
"	push       { r3  - r11, lr }   \n"
#if __FPU_USED
"	vpush      { s16 - s31 }       \n"
#endif
#endif
"	mov   r1,    sp                \n"

"priv_tsk_save:                    \n"

"	ldr   r0,   =System            \n"
"	ldr   r0,  [ r0, %[cur] ]      \n"
"	str   r1,  [ r0, %[sp] ]       \n"
"	bl    core_tsk_handler         \n"
"	ldr   r1,  [ r0, %[sp] ]       \n"
#if __CORTEX_M < 3
"	cmp   r1,   #0                 \n"
"	beq   priv_tsk_start           \n"
#else
"	cbz   r1,    priv_tsk_start    \n"
#endif
"	mov   sp,    r1                \n"
#if __CORTEX_M < 3
"	add   r1,   #20                \n"
"	ldm   r1!, { r3  - r7 }        \n"
"	mov   r8,    r3                \n"
"	mov   r9,    r4                \n"
"	mov   r10,   r5                \n"
"	mov   r11,   r6                \n"
"	mov   lr,    r7                \n"
"	sub   r1,   #40                \n"
"	ldm   r1!, { r3  - r7 }        \n"
"	add   sp,   #40                \n"
#else
#if __FPU_USED
"	vpop       { s16 - s31 }       \n"
#endif
"	pop        { r3  - r11, lr }   \n"
#endif
"	msr   PRIMASK, r3              \n"

"priv_ctx_exit:                    \n"

"	bx    lr                       \n"

"priv_tsk_start:                   \n"

"	ldr   r1,  [ r0, %[top] ]      \n"
"	mov   sp,    r1                \n"
"	ldr   r3,  [ r0, %[state] ]    \n"
"	blx   r3                       \n"
	
"	.thumb_func                    \n"
"core_tsk_break:                   \n"

"	movs  r1,   #0                 \n"
"	b     priv_tsk_save            \n"

::	[cur]   "n" (offsetof(sys_t, cur)),
	[state] "n" (offsetof(tsk_t, state)),
	[sp]    "n" (offsetof(tsk_t, sp)),
	[top]   "n" (offsetof(tsk_t, top))
	);
}

/* -------------------------------------------------------------------------- */

#endif // __ARMCOMPILER_VERSION
