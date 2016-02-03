/******************************************************************************
 * @file    stm32f4_discovery_rand.h
 * @author  Rajmund Szymanski
 * @date    10.11.2015
 * @brief   This file contains definitions for STM32F4-Discovery Kit.
 ******************************************************************************/

#pragma once

/* -------------------------------------------------------------------------- */

#include <stm32f4_io.h>

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

// start random number generator

static inline
void srand( unsigned rnd )
{
	BB(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN) = 1;

	RNG->CR = RNG_CR_RNGEN;
}

/* -------------------------------------------------------------------------- */

// get next random number

static inline
int rand( void )
{
	while ((RNG->SR & RNG_SR_DRDY) == 0);

	return RNG->DR;
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
}
#endif
