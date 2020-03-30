/******************************************************************************

    @file    IntrOS: osport.c
    @author  Rajmund Szymanski
    @date    29.03.2020
    @brief   IntrOS port file for LM4F uC.

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

void port_sys_init( void )
{
#if HW_TIMER_SIZE == 0

/******************************************************************************
 Non-tick-less mode: configuration of system timer
 It must generate interrupts with frequency OS_FREQUENCY
*******************************************************************************/

	#if (CPU_FREQUENCY)/(OS_FREQUENCY)-1 <= SysTick_LOAD_RELOAD_Msk

	SysTick_Config((CPU_FREQUENCY)/(OS_FREQUENCY));

	#elif defined(ST_FREQUENCY) && \
	    (ST_FREQUENCY)/(OS_FREQUENCY)-1 <= SysTick_LOAD_RELOAD_Msk

	NVIC_SetPriority(SysTick_IRQn, 0xFF);

	SysTick->LOAD = (ST_FREQUENCY)/(OS_FREQUENCY)-1;
	SysTick->VAL  = 0U;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk;

	#else
	#error Incorrect SysTick frequency!
	#endif

/******************************************************************************
 End of configuration
*******************************************************************************/

#else //HW_TIMER_SIZE

/******************************************************************************
 Tick-less mode: configuration of system timer
 It must be rescaled to frequency OS_FREQUENCY
*******************************************************************************/

	#if (CPU_FREQUENCY)/(OS_FREQUENCY)-1 > UINT16_MAX
	#error Incorrect Timer frequency!
	#endif

	SYSCTL->RCGCWTIMER |= SYSCTL_RCGCWTIMER_R0;
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	NVIC_SetPriority(WTIMER0A_IRQn, 0xFF);
	NVIC_EnableIRQ(WTIMER0A_IRQn);
	#else
	__ISB();
	#endif
	WTIMER0->CFG   = TIMER_CFG_16_BIT; // WTIMER is 32 bit
	WTIMER0->TAMR  = TIMER_TAMR_TAMR_PERIOD;
	#if HW_TIMER_SIZE > OS_TIMER_SIZE
	WTIMER0->TAILR = CNT_MAX;
	#endif
	WTIMER0->TAPR  = (CPU_FREQUENCY)/(OS_FREQUENCY)-1;
	WTIMER0->CTL   = TIMER_CTL_TAEN;
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	WTIMER0->IMR   = TIMER_IMR_TATOIM;
	#endif

/******************************************************************************
 End of configuration
*******************************************************************************/

#endif//HW_TIMER_SIZE
}

/* -------------------------------------------------------------------------- */

#if HW_TIMER_SIZE == 0

/******************************************************************************
 Non-tick-less mode: interrupt handler of system timer
*******************************************************************************/

void SysTick_Handler( void )
{
	SysTick->CTRL;
	core_sys_tick();
}

/******************************************************************************
 End of the handler
*******************************************************************************/

#else //HW_TIMER_SIZE

/******************************************************************************
 Tick-less mode: interrupt handler of system timer
*******************************************************************************/

#if HW_TIMER_SIZE < OS_TIMER_SIZE

void WTIMER0A_Handler( void )
{
//	if (WTIMER0->MIS & TIMER_MIS_TATOMIS)
	{
		WTIMER0->ICR = TIMER_ICR_TATOCINT;
		core_sys_tick();
		__ISB(); // delay because of GNUCC
	}
}

#endif

/******************************************************************************
 End of the handler
*******************************************************************************/

/******************************************************************************
 Tick-less mode: return current system time
*******************************************************************************/

#if HW_TIMER_SIZE < OS_TIMER_SIZE

cnt_t port_sys_time( void )
{
	cnt_t    cnt;
	uint32_t tck;

	cnt = System.cnt;
	tck = -WTIMER0->TAV;

	if (WTIMER0->MIS & TIMER_MIS_TATOMIS)
	{
		tck = -WTIMER0->TAV;
		cnt += (cnt_t)(1) << (HW_TIMER_SIZE);
	}

	return cnt + tck;
}

#endif

/******************************************************************************
 End of the function
*******************************************************************************/

#endif//HW_TIMER_SIZE

/* -------------------------------------------------------------------------- */
