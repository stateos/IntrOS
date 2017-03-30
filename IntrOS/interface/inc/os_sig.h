/******************************************************************************

    @file    IntrOS: os_sig.h
    @author  Rajmund Szymanski
    @date    30.03.2017
    @brief   This file contains definitions for IntrOS.

 ******************************************************************************

    IntrOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of IntrOS distribution.

    IntrOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    IntrOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef __INTROS_SIG_H
#define __INTROS_SIG_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : signal                                                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

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

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _SIG_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a signal object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 * Return            : signal object                                                                                  *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _SIG_INIT( type ) { 0, (type)&sigMASK }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_SIG                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a signal object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : name of a pointer to signal object                                                             *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_SIG( sig, type )                     \
                       sig_t sig##__sig = _SIG_INIT( type ); \
                       sig_id sig = & sig##__sig

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_SIG                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static signal object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : name of a pointer to signal object                                                             *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_SIG( sig, type )                     \
                static sig_t sig##__sig = _SIG_INIT( type ); \
                static sig_id sig = & sig##__sig

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SIG_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a signal object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 * Return            : signal object                                                                                  *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                SIG_INIT( type ) \
                      _SIG_INIT( type )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SIG_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a signal object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 * Return            : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                SIG_CREATE( type ) \
             & (sig_t) SIG_INIT( type )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the signal object has been released                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void sig_wait( sig_t *sig );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_take                                                                                       *
 *                                                                                                                    *
 * Description       : don't wait until the signal object has been released                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : signal object was successfully released                                                        *
 *   E_FAILURE       : signal object can't be released immediately                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned sig_take( sig_t *sig );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_give                                                                                       *
 *                                                                                                                    *
 * Description       : resume one (sigClear) or all (sigProtect) tasks that are waiting on the signal object          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void sig_give( sig_t *sig );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_clear                                                                                      *
 *                                                                                                                    *
 * Description       : reset the signal object                                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void sig_clear( sig_t *sig );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Signal                                                                                         *
 *                                                                                                                    *
 * Description       : create and initilize a signal object                                                           *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal (default)                                                     *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Signal : public __sig
{
	explicit
	Signal( const unsigned _type = sigClear ): __sig _SIG_INIT(0) { type = _type; }

	void     wait ( void ) {        sig_wait (this); }
	unsigned take ( void ) { return sig_take (this); }
	void     give ( void ) {        sig_give (this); }
	void     clear( void ) {        sig_clear(this); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_SIG_H
