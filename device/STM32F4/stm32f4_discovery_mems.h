/******************************************************************************
 * @file    stm32f4_discovery_mems.h
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

#define MEMS_DUMMY  ((uint8_t)0x00)
#define MEMS_MULTI  ((uint8_t)0x40)
#define MEMS_RWCMD  ((uint8_t)0x80)

#define MEMS_CS     BITBAND(GPIOE->ODR)[3]

/* -------------------------------------------------------------------------- */

static inline
void MEMS_Config( void )
{
	GPIO_Init(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Alternate_50MHz | GPIO_PullDown | GPIO_AF_SPI1); // SCK, MISO, MOSI
	GPIO_Init(GPIOE, GPIO_Pin_0 | GPIO_Pin_1,              GPIO_Input);                                          // INT1, INT2
	GPIO_Init(GPIOE, GPIO_Pin_3,                           GPIO_Output_PushPull | GPIO_Set);                     // CS

	BB(RCC->APB2ENR, RCC_APB2ENR_SPI1EN) = 1;

	SPI1->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_BR_0;
	SPI1->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_BR_0 | SPI_CR1_SPE;
}

/* -------------------------------------------------------------------------- */

static inline
uint8_t MEMS_Send( uint8_t val )
{
	while (BB(SPI1->SR, SPI_SR_TXE)  == 0);
	SPI1->DR = val;
	while (BB(SPI1->SR, SPI_SR_RXNE) == 0);
	return SPI1->DR;
}

/* -------------------------------------------------------------------------- */

static inline
uint8_t MEMS_Read( uint8_t reg )
{
	uint8_t val;
	MEMS_CS = 0;
	      MEMS_Send(MEMS_RWCMD | reg);
	val = MEMS_Send(MEMS_DUMMY);
	MEMS_CS = 1;
	return val;
}

/* -------------------------------------------------------------------------- */

static inline
void MEMS_Write( uint8_t reg, uint8_t val )
{
	MEMS_CS = 0;
	MEMS_Send(reg);
	MEMS_Send(val);
	MEMS_CS = 1;
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
