/******************************************************************************
 * @file    stm32f4_discovery_udid.h
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

/* UDID --------------------------------------------------------------------- */

#ifndef UDID

#define UDID_BASE  0x1FFF7A10U

typedef struct {
	__I uint32_t ID[3];
} UDID_TypeDef;

#define UDID  ((UDID_TypeDef*)UDID_BASE)

#endif // UDID

/* -------------------------------------------------------------------------- */

// get udid crc

static inline
uint32_t UDID_Get( void )
{
	BB(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN) = 1;

	CRC->CR = CRC_CR_RESET;
	CRC->DR = UDID->ID[0];
	CRC->DR = UDID->ID[1];
	CRC->DR = UDID->ID[2];

	return ~CRC->DR;
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
