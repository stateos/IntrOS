/******************************************************************************

    @file    IntrOS: osport.c
    @author  Rajmund Szymanski
    @date    01.01.2018
    @brief   IntrOS port file for STM32F4 uC.

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

#include <oskernel.h>

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

	#if (CPU_FREQUENCY)/(OS_FREQUENCY)/2-1 > UINT16_MAX
	#error Incorrect Timer frequency!
	#endif

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	NVIC_SetPriority(TIM2_IRQn, 0xFF);
	NVIC_EnableIRQ(TIM2_IRQn);
	#else
	__ISB();
	#endif
	TIM2->PSC  = (CPU_FREQUENCY)/(OS_FREQUENCY)/2-1;
	TIM2->EGR  = TIM_EGR_UG;
	TIM2->CR1  = TIM_CR1_CEN;
	#if HW_TIMER_SIZE < OS_TIMER_SIZE
	TIM2->DIER = TIM_DIER_UIE;
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

void TIM2_IRQHandler( void )
{
//	if (TIM2->SR & TIM_SR_UIF)
	{
		TIM2->SR = ~TIM_SR_UIF;
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
	tck = TIM2->CNT;

	if (TIM2->SR & TIM_SR_UIF)
	{
		tck = TIM2->CNT;
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
