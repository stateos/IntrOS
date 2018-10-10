/******************************************************************************

    @file    IntrOS: ossignal.h
    @author  Rajmund Szymanski
    @date    10.10.2018
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
	unsigned flags; // pending signals
	unsigned mask;  // protection mask
};

/******************************************************************************
 *
 * Name              : _SIG_INIT
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   mask            : protection mask of signal object
 *
 * Return            : signal object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _SIG_INIT( _mask ) { 0, _mask }

/******************************************************************************
 *
 * Name              : _VA_SIG
 *
 * Description       : calculate protection mask from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_SIG( _mask ) ( _mask + 0 )

/******************************************************************************
 *
 * Name              : OS_SIG
 *
 * Description       : define and initialize a signal object
 *
 * Parameters
 *   sig             : name of a pointer to signal object
 *   mask            : (optional) protection mask; default: 0
 *
 ******************************************************************************/

#define             OS_SIG( sig, ... )                                      \
                       sig_t sig##__sig = _SIG_INIT( _VA_SIG(__VA_ARGS__) ); \
                       sig_id sig = & sig##__sig

/******************************************************************************
 *
 * Name              : static_SIG
 *
 * Description       : define and initialize a static signal object
 *
 * Parameters
 *   sig             : name of a pointer to signal object
 *   mask            : (optional) protection mask; default: 0
 *
 ******************************************************************************/

#define         static_SIG( sig, ... )                                      \
                static sig_t sig##__sig = _SIG_INIT( _VA_SIG(__VA_ARGS__) ); \
                static sig_id sig = & sig##__sig

/******************************************************************************
 *
 * Name              : SIG_INIT
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   mask            : (optional) protection mask; default: 0
 *
 * Return            : signal object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SIG_INIT( ... ) \
                      _SIG_INIT( _VA_SIG(__VA_ARGS__) )
#endif

/******************************************************************************
 *
 * Name              : SIG_CREATE
 * Alias             : SIG_NEW
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   mask            : (optional) protection mask; default: 0
 *
 * Return            : pointer to signal object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SIG_CREATE( ... ) \
           (sig_t[]) { SIG_INIT  ( _VA_SIG(__VA_ARGS__) ) }
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
 *   mask            : protection mask of signal object
 *
 * Return            : none
 *
 ******************************************************************************/

void sig_init( sig_t *sig, unsigned mask );

/******************************************************************************
 *
 * Name              : sig_take
 * Alias             : sig_tryWait
 *
 * Description       : check signal object for the given signal number
 *
 * Parameters
 *   sig             : pointer to signal object
 *   num             : signal number
 *
 * Return
 *   E_SUCCESS       : required signal number has been set
 *   E_FAILURE       : required signal number has not been set, try again
 *
 ******************************************************************************/

unsigned sig_take( sig_t *sig, unsigned num );

__STATIC_INLINE
unsigned sig_tryWait( sig_t *sig, unsigned num ) { return sig_take(sig, num); }

/******************************************************************************
 *
 * Name              : sig_wait
 *
 * Description       : wait indefinitely until the given signal number has been set
 *
 * Parameters
 *   sig             : pointer to signal object
 *   num             : signal number
 *
 * Return            : none
 *
 ******************************************************************************/

void sig_wait( sig_t *sig, unsigned num );

/******************************************************************************
 *
 * Name              : sig_give
 * Alias             : sig_set
 *
 * Description       : set given signal number in the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *   num             : signal number
 *
 * Return            : none
 *
 ******************************************************************************/

void sig_give( sig_t *sig, unsigned num );

__STATIC_INLINE
void sig_set( sig_t *sig, unsigned num ) { sig_give(sig, num); }

/******************************************************************************
 *
 * Name              : sig_clear
 *
 * Description       : reset given signal number in the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *   num             : signal number
 *
 * Return            : none
 *
 ******************************************************************************/

void sig_clear( sig_t *sig, unsigned num );

/******************************************************************************
 *
 * Name              : sig_get
 *
 * Description       : get given signal state from signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return            : signal state in signal object
 *
 ******************************************************************************/

bool sig_get( sig_t *sig, unsigned num );

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
 *   mask            : protection mask of signal object
 *
 ******************************************************************************/

struct Signal : public __sig
{
	Signal( const unsigned _mask = 0 ): __sig _SIG_INIT(_mask) {}

	unsigned take   ( unsigned num ) { return sig_take   (this, num); }
	unsigned tryWait( unsigned num ) { return sig_tryWait(this, num); }
	void     wait   ( unsigned num ) {        sig_wait   (this, num); }
	void     give   ( unsigned num ) {        sig_give   (this, num); }
	void     set    ( unsigned num ) {        sig_set    (this, num); }
	void     clear  ( unsigned num ) {        sig_clear  (this, num); }
	bool     get    ( unsigned num ) { return sig_get    (this, num); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_SIG_H
