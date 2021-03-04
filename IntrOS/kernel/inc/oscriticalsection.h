/******************************************************************************

    @file    IntrOS: oscriticalsection.h
    @author  Rajmund Szymanski
    @date    02.03.2021
    @brief   This file contains definitions for IntrOS.

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

#ifndef __INTROS_CRI_H
#define __INTROS_CRI_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : core_set_lock
 *
 * Description       : disable interrupts
 *
 * Parameters        : none
 *
 * Return            : previous interrupts state
 *
 * Note              : for internal use
 *
 ******************************************************************************/

__STATIC_INLINE
lck_t core_set_lock( void )
{
	lck_t prv = port_get_lock();
	port_set_lock();
	__COMPILER_BARRIER();
	return prv;
}

/******************************************************************************
 *
 * Name              : core_clr_lock
 *
 * Description       : enable interrupts
 *
 * Parameters        : none
 *
 * Return            : previous interrupts state
 *
 * Note              : for internal use
 *
 ******************************************************************************/

__STATIC_INLINE
lck_t core_clr_lock( void )
{
	lck_t prv = port_get_lock();
	port_clr_lock();
	__COMPILER_BARRIER();
	return prv;
}

/******************************************************************************
 *
 * Name              : core_put_lock
 *
 * Description       : restore interrupts state
 *
 * Parameters
 *   lck             : interrupts state
 *
 * Return            : none
 *
 * Note              : for internal use
 *
 ******************************************************************************/

__STATIC_INLINE
void core_put_lock( lck_t lck )
{
	__COMPILER_BARRIER();
	port_put_lock(lck);
}

/******************************************************************************
 *
 * Name              : core_get_lock
 *
 * Description       : return current interrupts state
 *
 * Parameters        : none
 *
 * Return            : current interrupts state
 *
 * Note              : for internal use
 *
 ******************************************************************************/

__STATIC_INLINE
lck_t core_get_lock( void )
{
	return port_get_lock();
}

/******************************************************************************
 *
 * Name              : sys_lock
 *
 * Description       : save interrupts state then disable interrupts
 *                   / enter into critical section
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

#define                sys_lock() \
                       do { lck_t __LOCK = core_set_lock()

/******************************************************************************
 *
 * Name              : sys_unlock
 *
 * Description       : restore saved interrupts state
 *                   / exit from critical section
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

#define                sys_unlock() \
                       core_put_lock(__LOCK); } while (0)

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : CriticalSection
 *
 * Description       : create and initialize a critical section guard object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct CriticalSection
{
	 CriticalSection( void ) { lck_ = core_set_lock(); }
	~CriticalSection( void ) { core_put_lock(lck_); }

	CriticalSection( CriticalSection&& ) = delete;
	CriticalSection( const CriticalSection& ) = delete;
	CriticalSection& operator=( CriticalSection&& ) = delete;
	CriticalSection& operator=( const CriticalSection& ) = delete;

	private:
	lck_t lck_;
};

/******************************************************************************
 *
 * Class             : Lock
 *
 * Description       : create and initialize a guard object
 *
 * Constructor parameters
 *   T               : guard class
 *
 ******************************************************************************/

template<class T>
struct Lock
{
	 Lock( T &_lck ): lck_(_lck) { lck_.lock(); }
	~Lock( void )                { lck_.unlock(); }

	Lock( Lock&& ) = default;
	Lock( const Lock& ) = delete;
	Lock& operator=( Lock&& ) = delete;
	Lock& operator=( const Lock& ) = delete;

	private:
	T &lck_;
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_CRI_H
