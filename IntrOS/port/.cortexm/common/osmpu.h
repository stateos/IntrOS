/******************************************************************************

    @file    IntrOS: osmpu.h
    @author  Rajmund Szymanski
    @date    21.02.2021
    @brief   This file defines set of memory protection unit functions for IntrOS.

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

#ifndef __INTROSMPU_H
#define __INTROSMPU_H

#include "osport.h"

#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1)

#if defined (DEBUG) && !defined (OS_GUARD_SIZE)
#define OS_GUARD_SIZE    32 /* ARM_MPU_REGION_SIZE_32B */
#endif

#ifndef __MPU_USED
#if defined (OS_GUARD_SIZE) && ((OS_GUARD_SIZE) >= 32) && ((OS_GUARD_SIZE) == ((OS_GUARD_SIZE) & ~((OS_GUARD_SIZE) - 1)))
#define __MPU_USED        1
#endif
#elif   __MPU_USED
#error  __MPU_USED is an internal os definition!
#endif//__MPU_USED

#if defined (__MPU_USED) && (__MPU_USED == 1)

#ifdef __cplusplus
extern "C" {
#endif

__STATIC_INLINE
uint32_t priv_mpu_region( uint32_t size )
{
	uint32_t region = 4;
	while (size > 32) { size /= 2; region++; }
	return region;
}

/******************************************************************************
 *
 * Name              : port_mpu_disable
 *
 * Description       : disable the memory protection unit
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void port_mpu_disable( void )
{
	ARM_MPU_Disable();
}

/******************************************************************************
 *
 * Name              : port_mpu_enable
 *
 * Description       : configure the memory protection unit for use
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void port_mpu_enable( void )
{
	ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

/******************************************************************************
 *
 * Name              : port_mpu_setRegion
 *
 * Description       : configure memory ptotection unit region
 *
 * Parameters
 *   reg             : region number
 *   mem             : memory address
 *   xn              : instruction access disable bit (0 / 1)
 *   ap              : data access permissions (ARM_MPU_AP_*)
 *   access          : memory access attribution (ARM_MPU_ACCESS_*)
 *   subregion       : sub-region disable bit field
 *   size            : region size (ARM_MPU_REGION_SIZE_*)
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void port_mpu_setRegion( const uint32_t  reg,
                         const uintptr_t mem,
                         const uint32_t  xn,
                         const uint32_t  ap,
                         const uint32_t  access,
                         const uint32_t  subregion,
                         const size_t    size )
{
	uint32_t rasr = ARM_MPU_RASR_EX(xn, ap, access, subregion, size);
	uint32_t rbar = ARM_MPU_RBAR(reg, mem);
	ARM_MPU_SetRegion(rbar, rasr);
}

/******************************************************************************
 *
 * Name              : port_mpu_nullptrLock
 *
 * Description       : prevention the use of the NULL / nullptr
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void port_mpu_nullptrLock( void )
{
	port_mpu_setRegion(1U, 0U, SET, ARM_MPU_AP_NONE, 0U, 0U, priv_mpu_region(OS_GUARD_SIZE));
}

/******************************************************************************
 *
 * Name              : port_mpu_stackLock
 *
 * Description       : prevention the stack overflow
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void port_mpu_stackLock( void )
{
	port_mpu_setRegion(0U, 0U, SET, ARM_MPU_AP_RO, 0U, 0U, priv_mpu_region(OS_GUARD_SIZE));
}

/******************************************************************************
 *
 * Name              : port_mpu_stackUpdate
 *
 * Description       : updated prevention the stack overflow
 *
 * Parameters
 *   stk             : stack base address
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void port_mpu_stackUpdate( const void *stk )
{
	MPU->RBAR = ARM_MPU_RBAR(0U, ((uintptr_t)stk + (size_t)(OS_GUARD_SIZE) - 1) & ~((size_t)(OS_GUARD_SIZE) - 1));
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__MPU_USED
#endif//__MPU_PRESENT
#endif//__INTROSMPU_H
