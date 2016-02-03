/******************************************************************************
 * @file    stm32f4_discovery_btns.h
 * @author  Rajmund Szymanski
 * @date    10.11.2015
 * @brief   This file contains definitions for STM32F4-Discovery Kit.
 ******************************************************************************/

#pragma once

/* -------------------------------------------------------------------------- */

#include <stm32f4_io.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define BTN BITBAND(GPIOA->IDR)[0] // user button

/* -------------------------------------------------------------------------- */

// config user button (PA0)

static inline
void BTN_Config( void )
{
	GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Input_PullDown);
}

/* -------------------------------------------------------------------------- */

// config user button (PA0) as a source of exti interrupt

static inline
void BTN_EXTI_Config( void )
{
	GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Input_NoPull);

	BB(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN) = 1;

//	SYSCFG->EXTICR[0]            |= SYSCFG_EXTICR1_EXTI0_PA; // SYSCFG->EXTI0(PIN_0) = 0(PORT_A)
	BB(EXTI->IMR,  EXTI_IMR_MR0)  = 1; // interrupt
//	BB(EXTI->EMR,  EXTI_EMR_MR0)  = 0; // event
	BB(EXTI->RTSR, EXTI_RTSR_TR0) = 1; // rising trigger
//	BB(EXTI->FTSR, EXTI_FTSR_TR0) = 0; // falling trigger

	NVIC_EnableIRQ(EXTI0_IRQn);
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/* -------------------------------------------------------------------------- */

class Button
{
public:
	Button( void ) { BTN_Config(); }
	
	unsigned operator ()( void ) { return BTN; }
};

/* -------------------------------------------------------------------------- */

#endif
