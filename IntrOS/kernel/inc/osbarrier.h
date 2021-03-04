/******************************************************************************

    @file    IntrOS: osbarrier.h
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

#ifndef __INTROS_BAR_H
#define __INTROS_BAR_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : barrier
 *
 ******************************************************************************/

typedef struct __bar bar_t, * const bar_id;

struct __bar
{
	unsigned count; // barrier's current value
	unsigned limit; // barrier's value limit

	unsigned signal;
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _BAR_INIT
 *
 * Description       : create and initialize a barrier object
 *
 * Parameters
 *   limit           : number of tasks that must call bar_wait function to release the barrier object
 *
 * Return            : barrier object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _BAR_INIT( _limit ) { _limit, _limit, 0 }

/******************************************************************************
 *
 * Name              : OS_BAR
 *
 * Description       : define and initialize a barrier object
 *
 * Parameters
 *   bar             : name of a pointer to barrier object
 *   limit           : number of tasks that must call bar_wait function to release the barrier object
 *
 ******************************************************************************/

#define             OS_BAR( bar, limit )                     \
                       bar_t bar##__bar = _BAR_INIT( limit ); \
                       bar_id bar = & bar##__bar

/******************************************************************************
 *
 * Name              : static_BAR
 *
 * Description       : define and initialize a static barrier object
 *
 * Parameters
 *   bar             : name of a pointer to barrier object
 *   limit           : number of tasks that must call bar_wait function to release the barrier object
 *
 ******************************************************************************/

#define         static_BAR( bar, limit )                     \
                static bar_t bar##__bar = _BAR_INIT( limit ); \
                static bar_id bar = & bar##__bar

/******************************************************************************
 *
 * Name              : BAR_INIT
 *
 * Description       : create and initialize a barrier object
 *
 * Parameters
 *   limit           : number of tasks that must call bar_wait function to release the barrier object
 *
 * Return            : barrier object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                BAR_INIT( limit ) \
                      _BAR_INIT( limit )
#endif

/******************************************************************************
 *
 * Name              : BAR_CREATE
 * Alias             : BAR_NEW
 *
 * Description       : create and initialize a barrier object
 *
 * Parameters
 *   limit           : number of tasks that must call bar_wait function to release the barrier object
 *
 * Return            : pointer to barrier object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                BAR_CREATE( limit ) \
           (bar_t[]) { BAR_INIT  ( limit ) }
#define                BAR_NEW \
                       BAR_CREATE
#endif

/******************************************************************************
 *
 * Name              : bar_init
 *
 * Description       : initialize a barrier object
 *
 * Parameters
 *   bar             : pointer to barrier object
 *   limit           : number of tasks that must call bar_wait function to release the barrier object
 *
 * Return            : none
 *
 ******************************************************************************/

void bar_init( bar_t *bar, unsigned limit );

/******************************************************************************
 *
 * Name              : bar_wait
 *
 * Description       : wait indefinitely until the barrier object has been released
 *
 * Parameters
 *   bar             : pointer to barrier object
 *
 * Return            : none
 *
 ******************************************************************************/

void bar_wait( bar_t *bar );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : Barrier
 *
 * Description       : create and initialize a barrier object
 *
 * Constructor parameters
 *   limit           : number of tasks that must call wait function to release the barrier object
 *
 ******************************************************************************/

struct Barrier : public __bar
{
	constexpr
	Barrier( const unsigned _limit ): __bar _BAR_INIT(_limit) {}

	Barrier( Barrier&& ) = default;
	Barrier( const Barrier& ) = delete;
	Barrier& operator=( Barrier&& ) = delete;
	Barrier& operator=( const Barrier& ) = delete;

	void wait( void ) { bar_wait(this); }
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_BAR_H
