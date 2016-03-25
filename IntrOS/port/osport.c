/******************************************************************************

    @file    IntrOS: osport.c
    @author  Rajmund Szymanski
    @date    03.03.2016
    @brief   IntrOS port file for Cortex-Mx uC.

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
/******************************************************************************
 Put here configuration of system timer

 Example for TIM1 (STM32F4):

	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0xFF);
	NVIC_EnableIRQ  (TIM1_UP_TIM10_IRQn);
	BB(RCC->APB2ENR, RCC_APB2ENR_TIM1EN) = 1;

	TIM1->PSC  = CPU_FREQUENCY / 10000 - 1;
	TIM1->ARR  = 10000 /  OS_FREQUENCY - 1;
	TIM1->EGR  = TIM_EGR_UG;
	TIM1->CR1  = TIM_CR1_CEN;
	TIM1->DIER = TIM_DIER_UIE;

*******************************************************************************/

	SysTick_Config(CPU_FREQUENCY/OS_FREQUENCY);
}

/* -------------------------------------------------------------------------- */

/******************************************************************************
 Put here the procedure of interrupt handler of system timer for non-tick-less mode

 Example for TIM1 (STM32F4):

void TIM1_UP_TIM10_IRQHandler( void )
{
	TIM1->SR = 0; // clear timer's status register

	Counter++;
}

*******************************************************************************/

void SysTick_Handler( void )
{
	Counter++;
}

/* -------------------------------------------------------------------------- */
