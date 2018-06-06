/******************************************************************************

    @file    IntrOS: ossignal.h
    @author  Rajmund Szymanski
    @date    05.06.2018
    @brief   This file contains definitions for IntrOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

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

#ifndef __INTROS_SIG_H
#define __INTROS_SIG_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : signal
 *
 ******************************************************************************/

typedef struct __sig sig_t, * const sig_id;

struct __sig
{
	unsigned flag;  // signal's current value
	unsigned type;  // signal type: sigClear, sigProtect
};

/* -------------------------------------------------------------------------- */

#define sigClear     ( 0U << 0 ) // auto clearing signal
#define sigProtect   ( 1U << 0 ) // protected signal
#define sigMASK      ( 1U )

/******************************************************************************
 *
 * Name              : _SIG_INIT
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : signal object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _SIG_INIT( type ) { 0, (type)&sigMASK }

/******************************************************************************
 *
 * Name              : OS_SIG
 *
 * Description       : define and initialize a signal object
 *
 * Parameters
 *   sig             : name of a pointer to signal object
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 ******************************************************************************/

#define             OS_SIG( sig, type )                     \
                       sig_t sig##__sig = _SIG_INIT( type ); \
                       sig_id sig = & sig##__sig

/******************************************************************************
 *
 * Name              : static_SIG
 *
 * Description       : define and initialize a static signal object
 *
 * Parameters
 *   sig             : name of a pointer to signal object
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 ******************************************************************************/

#define         static_SIG( sig, type )                     \
                static sig_t sig##__sig = _SIG_INIT( type ); \
                static sig_id sig = & sig##__sig

/******************************************************************************
 *
 * Name              : SIG_INIT
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : signal object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SIG_INIT( type ) \
                      _SIG_INIT( type )
#endif

/******************************************************************************
 *
 * Name              : SIG_CREATE
 * Alias             : SIG_NEW
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : pointer to signal object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SIG_CREATE( type ) \
             & (sig_t) SIG_INIT  ( type )
#define                SIG_NEW \
                       SIG_CREATE
#endif

/******************************************************************************
 *
 * Name              : sig_init
 *
 * Description       : initialize a signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : none
 *
 ******************************************************************************/

void sig_init( sig_t *sig, unsigned type );

/******************************************************************************
 *
 * Name              : sig_wait
 *
 * Description       : wait indefinitely until the signal object has been released
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return            : none
 *
 ******************************************************************************/

void sig_wait( sig_t *sig );

/******************************************************************************
 *
 * Name              : sig_take
 *
 * Description       : check if the signal object has been released
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return
 *   E_SUCCESS       : signal object was successfully released
 *   E_FAILURE       : signal object was not released
 *
 ******************************************************************************/

unsigned sig_take( sig_t *sig );

/******************************************************************************
 *
 * Name              : sig_give
 *
 * Description       : release the signal object
 *                     resume one (sigClear) or all (sigProtect) tasks that are waiting on the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return            : none
 *
 ******************************************************************************/

void sig_give( sig_t *sig );

/******************************************************************************
 *
 * Name              : sig_clear
 *
 * Description       : reset the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return            : none
 *
 ******************************************************************************/

void sig_clear( sig_t *sig );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Signal
 *
 * Description       : create and initialize a signal object
 *
 * Constructor parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal (default)
 *                     sigProtect: protected signal
 *
 ******************************************************************************/

struct Signal : public __sig
{
	explicit
	Signal( const unsigned _type = sigClear ): __sig _SIG_INIT(_type) {}

	void     wait ( void ) {        sig_wait (this); }
	unsigned take ( void ) { return sig_take (this); }
	void     give ( void ) {        sig_give (this); }
	void     clear( void ) {        sig_clear(this); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_SIG_H
