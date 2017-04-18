/******************************************************************************

    @file    IntrOS: osport.h
    @author  Rajmund Szymanski
    @date    20.03.2017
    @brief   IntrOS port definitions for STM32F4 uC.

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

#ifndef __INTROSPORT_H
#define __INTROSPORT_H

#include <stdint.h>
#include <osconfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define GLUE( a, b, c )            a##b##c
#define  CAT( a, b, c )       GLUE(a, b, c)

/* -------------------------------------------------------------------------- */

#ifndef  __CORTEX_M
#error   osconfig.h: Include CMSIS device peripheral access layer header file!
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_TIMER
#define  OS_TIMER             0 /* os uses SysTick as system timer            */
#endif

/* -------------------------------------------------------------------------- */

#if      OS_TIMER

#define  OS_TIM            CAT(TIM,OS_TIMER,)
#define  OS_TIM_CLK_ENABLE CAT(RCC_APB1ENR_TIM,OS_TIMER,EN)
#define  OS_TIM_IRQn       CAT(TIM,OS_TIMER,_IRQn)
#define  OS_TIM_IRQHandler CAT(TIM,OS_TIMER,_IRQHandler)

#define  Counter           OS_TIM->CNT

#endif

/* -------------------------------------------------------------------------- */

#ifndef CPU_FREQUENCY
#error   osconfig.h: Undefined CPU_FREQUENCY value!
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_FREQUENCY

#if      OS_TIMER
#define  OS_FREQUENCY   1000000 /* Hz */
#else
#define  OS_FREQUENCY      1000 /* Hz */
#endif

#else

#if     (OS_TIMER == 0) && (OS_FREQUENCY > 1000)
#error   osconfig.h: Incorrect OS_FREQUENCY value!
#endif

#endif //OS_FREQUENCY

/* -------------------------------------------------------------------------- */

#define  ST_FREQUENCY (CPU_FREQUENCY/8) /* alternate clock source for SysTick */

/* -------------------------------------------------------------------------- */

#ifndef  OS_STACK_SIZE
#define  OS_STACK_SIZE      256 /* default task stack size in bytes           */
#endif

/* -------------------------------------------------------------------------- */

#ifndef  OS_ASSERT
#define  OS_ASSERT            0 /* don't include standard assertions          */
#endif

#if     (OS_ASSERT == 0)
#ifndef  NDEBUG
#define  NDEBUG
#endif
#endif

#ifndef  NDEBUG
#define  __ASSERT_MSG
#endif

#include <assert.h>

/* -------------------------------------------------------------------------- */

typedef  uint64_t             stk_t;

/* -------------------------------------------------------------------------- */

extern   stk_t              __initial_sp[];
#define  MAIN_TOP           __initial_sp

/* -------------------------------------------------------------------------- */

#if      defined(__CSMC__)

#ifndef  __CONSTRUCTOR
#define  __CONSTRUCTOR
#warning No compiler specific solution for __CONSTRUCTOR. __CONSTRUCTOR is ignored.
#endif

#define  __disable_irq()    __ASM("cpsid i")
#define  __enable_irq()     __ASM("cpsie i")

#else

#ifndef  __CONSTRUCTOR
#define  __CONSTRUCTOR      __attribute__((constructor))
#endif

#endif

/* -------------------------------------------------------------------------- */

#define  port_get_lock()          __get_PRIMASK()
#define  port_put_lock(state)     __set_PRIMASK(state)

#define  port_set_lock()          __disable_irq()
#define  port_clr_lock()          __enable_irq()

#define  port_sys_lock()       do { unsigned __LOCK = port_get_lock(); port_set_lock()
#define  port_sys_unlock()          port_put_lock(__LOCK); } while(0)

#define  port_isr_lock()       do { port_set_lock()
#define  port_isr_unlock()          port_clr_lock(); } while(0)

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROSPORT_H
