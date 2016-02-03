/******************************************************************************
 * @file    deprecated.h
 * @author  Rajmund Szymanski
 * @date    09.11.2015
 * @brief   
 ******************************************************************************/

#pragma once

/* -------------------------------------------------------------------------- */

#include <stdint.h>

/* -------------------------------------------------------------------------- */

typedef                 int32_t   s32;
typedef                 int16_t   s16;
typedef                 int8_t    s8;

typedef          const  int32_t  sc32;
typedef          const  int16_t  sc16;
typedef          const  int8_t   sc8;

typedef volatile        int32_t  vs32;
typedef volatile        int16_t  vs16;
typedef volatile        int8_t   vs8;

typedef volatile const  int32_t vsc32;
typedef volatile const  int16_t vsc16;
typedef volatile const  int8_t  vsc8;

typedef                uint32_t   u32;
typedef                uint16_t   u16;
typedef                uint8_t    u8;

typedef          const uint32_t  uc32;
typedef          const uint16_t  uc16;
typedef          const uint8_t   uc8;

typedef volatile       uint32_t  vu32;
typedef volatile       uint16_t  vu16;
typedef volatile       uint8_t   vu8;

typedef volatile const uint32_t vuc32;
typedef volatile const uint16_t vuc16;
typedef volatile const uint8_t  vuc8;

/* -------------------------------------------------------------------------- */
