/******************************************************************************

    @file    IntrOS: os_tmr.h
    @author  Rajmund Szymanski
    @date    01.03.2016
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
 * Name              : timer                                                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TMR                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TMR( tmr )                     \
                       tmr_t tmr##__tmr = _TMR_INIT(); \
                       tmr_id tmr = & tmr##__tmr

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TMR                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static timer object                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TMR( tmr )                     \
                static tmr_t tmr##__tmr = _TMR_INIT(); \
                static tmr_id tmr = & tmr##__tmr

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TMR_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : timer object                                                                                   *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                TMR_INIT() \
                      _TMR_INIT()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TMR_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TMR_CREATE() \
               &(tmr_t)TMR_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startUntil                                                                                 *
 *                                                                                                                    *
 * Description       : start/restart one-shot timer until given timepoint and then launch the callback procedure      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   time            : timepoint value                                                                                *
 *   proc            : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_startUntil( tmr_id tmr, unsigned time, fun_id proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startFor                                                                                   *
 *                                                                                                                    *
 * Description       : start/restart one-shot timer for given duration of time and then launch the callback procedure *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   delay           : duration of time (maximum number of ticks to countdownd)                                       *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   proc            : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_startFor( tmr_id tmr, unsigned delay, fun_id proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startPeriodic                                                                              *
 *                                                                                                                    *
 * Description       : start/restart periodic timer for given duration of time and then launch the callback procedure *
 *                     do this periodically                                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   period          : duration of time (maximum number of ticks to countdownd)                                       *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   proc            : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_startPeriodic( tmr_id tmr, unsigned period, fun_id proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the timer finishes countdown                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_wait( tmr_id tmr );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_take                                                                                       *
 *                                                                                                                    *
 * Description       : check if the timer finishes countdown                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : timer object successfully finished countdown                                                   *
 *   E_FAILURE       : timer object has not yet completed counting                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned tmr_take( tmr_id tmr );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_flip                                                                                       *
 *                                                                                                                    *
 * Description       : change callback procedure for current periodic timer (available in next period)                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   proc            : new callback procedure                                                                         *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in timer's callback procedure                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     tmr_flip( fun_id proc ) { Current->state = proc; }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_delay                                                                                      *
 *                                                                                                                    *
 * Description       : change delay time for current timer                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to countdownd)                                       *
 *                     0:         current timer is stopped even if it was periodic                                    *
 *                     otherwise: current timer is restarted even if it was one-shot                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in timer's callback procedure                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     tmr_delay( unsigned delay ) { Current->delay = delay; }

#ifdef __cplusplus
}
#endif
