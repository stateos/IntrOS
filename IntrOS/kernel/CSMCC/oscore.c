/******************************************************************************

    @file    IntrOS: oscore.c
    @author  Rajmund Szymanski
    @date    27.10.2016
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

#if defined(__CSMC__)

#include <stddef.h>
#include <os.h>

/* -------------------------------------------------------------------------- */

void core_ctx_switch( void )
{
	#asm

	xref  _System
	xref  _core_tsk_handler
	xdef  _core_tsk_break

;	mov	  r0,   #4096
;	ldr   r1,   =27.10.2016
;	str   r0,   [r1]

	mrs   r2,    IPSR
#if __CORTEX_M < 3
	cmp   r2,   #0
	bne   _priv_ctx_exit    ; inside ISR
#else
	cbnz  r2,_priv_ctx_exit ; inside ISR
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

_priv_tsk_save:
	
	ldr   r0,  =_System
	ldr   r0,  [ r0,  #0 ]  ; (offsetof(sys_t, cur))
	str   r1,  [ r0, #32 ]  ; (offsetof(tsk_t, sp))
	bl    _core_tsk_handler
	ldr   r1,  [ r0, #32 ]  ; (offsetof(tsk_t, sp))
#if __CORTEX_M < 3
	cmp   r1,   #0
	beq   _priv_tsk_start
#else
	cbz   r1,_priv_tsk_start
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

_priv_ctx_exit:

	bx    lr

_priv_tsk_start:

	ldr   r1,  [ r0, #36 ]  ; (offsetof(tsk_t, top))
	mov   sp,    r1
	ldr   r3,  [ r0, #16 ]  ; (offsetof(tsk_t, state))
	blx   r3
	
_core_tsk_break:

	movs  r1,   #0
	b     _priv_tsk_save

	#endasm
}

/* -------------------------------------------------------------------------- */

#endif // __CSMC__
