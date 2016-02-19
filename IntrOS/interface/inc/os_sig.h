/******************************************************************************

    @file    IntrOS: os_sig.h
    @author  Rajmund Szymanski
    @date    19.02.2016
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

#pragma once

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : signal                                                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define  sigClear    ( 0U )
#define  sigProtect  ( 1U )
#define  sigMASK     ( 1U )

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

#define     OS_SIG( sig, type )                             \
               sig_t sig##__sig = _SIG_INIT((type)&sigMASK); \
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

#define static_SIG( sig, type )                             \
        static sig_t sig##__sig = _SIG_INIT((type)&sigMASK); \
        static sig_id sig = & sig##__sig

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SIG_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize an signal object                                                          *
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

#define                SIG_INIT( type ) \
                      _SIG_INIT((type)&sigMASK)

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
               &(sig_t)SIG_INIT((type)&sigMASK)
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

              void     sig_wait( sig_id sig );

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

              unsigned sig_take( sig_id sig );

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

              void     sig_give( sig_id sig );

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

              void     sig_clear( sig_id sig );

#ifdef __cplusplus
}
#endif
