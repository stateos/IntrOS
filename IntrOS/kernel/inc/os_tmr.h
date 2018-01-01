/******************************************************************************

    @file    IntrOS: os_tmr.h
    @author  Rajmund Szymanski
    @date    30.12.2017
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

#ifndef __INTROS_TMR_H
#define __INTROS_TMR_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : timer
 *
 ******************************************************************************/

struct __tmr
{
	unsigned id;    // inherited from object
	tmr_t  * prev;  // inherited from object
	tmr_t  * next;  // inherited from object
	unsigned signal;

	fun_t  * state; // callback procedure
	cnt_t    start;
	cnt_t    delay;
	cnt_t    period;
};

/******************************************************************************
 *
 * Name              : _TMR_INIT
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : timer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _TMR_INIT( _state ) { 0, 0, 0, 0, _state, 0, 0, 0 }

/******************************************************************************
 *
 * Name              : OS_TMR
 *
 * Description       : define and initialize a timer object
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 ******************************************************************************/

#define             OS_TMR( tmr, state )                     \
                       tmr_t tmr##__tmr = _TMR_INIT( state ); \
                       tmr_id tmr = & tmr##__tmr

/******************************************************************************
 *
 * Name              : OS_TMR_DEF
 *
 * Description       : define and initialize complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *
 ******************************************************************************/

#define             OS_TMR_DEF( tmr )                            \
                       void tmr##__fun( void );                   \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun ); \
                       tmr_id tmr = & tmr##__tmr;                   \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TMR_UNTIL
 *
 * Description       : define, initialize and start complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   time            : timepoint value
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TMR_UNTIL( tmr, time )                               \
                       void tmr##__fun( void );                              \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );            \
                       tmr_id tmr = & tmr##__tmr;                              \
         __CONSTRUCTOR void tmr##__start( void ) { tmr_startUntil(tmr, time); } \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TMR_START
 *
 * Description       : define, initialize and start complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TMR_START( tmr, delay, period )                          \
                       void tmr##__fun( void );                                  \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                \
                       tmr_id tmr = & tmr##__tmr;                                  \
         __CONSTRUCTOR void tmr##__start( void ) { tmr_start(tmr, delay, period); } \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR
 *
 * Description       : define and initialize static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 ******************************************************************************/

#define         static_TMR( tmr, state )                     \
                static tmr_t tmr##__tmr = _TMR_INIT( state ); \
                static tmr_id tmr = & tmr##__tmr

/******************************************************************************
 *
 * Name              : static_TMR_DEF
 *
 * Description       : define and initialize static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *
 ******************************************************************************/

#define         static_TMR_DEF( tmr )                            \
                static void tmr##__fun( void );                   \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun ); \
                static tmr_id tmr = & tmr##__tmr;                   \
                static void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR_UNTIL
 *
 * Description       : define, initialize and start static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   time            : timepoint value
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TMR_UNTIL( tmr, time )                               \
                static void tmr##__fun( void );                              \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );            \
                static tmr_id tmr = & tmr##__tmr;                              \
  __CONSTRUCTOR static void tmr##__start( void ) { tmr_startUntil(tmr, time); } \
                static  void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR_START
 *
 * Description       : define, initialize and start static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TMR_START( tmr, delay, period )                          \
                static void tmr##__fun( void );                                  \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                \
                static tmr_id tmr = & tmr##__tmr;                                  \
  __CONSTRUCTOR static void tmr##__start( void ) { tmr_start(tmr, delay, period); } \
                static void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : TMR_INIT
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : timer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TMR_INIT( state ) \
                      _TMR_INIT( state )
#endif

/******************************************************************************
 *
 * Name              : TMR_CREATE
 * Alias             : TMR_NEW
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : pointer to timer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TMR_CREATE( state ) \
             & (tmr_t) TMR_INIT  ( state )
#define                TMR_NEW \
                       TMR_CREATE
#endif

/******************************************************************************
 *
 * Name              : tmr_this
 *
 * Description       : return current timer object
 *
 * Parameters        : none
 *
 * Return            : current timer object
 *
 ******************************************************************************/

__STATIC_INLINE
tmr_t *tmr_this( void ) { return (tmr_t *) System.cur; }

/******************************************************************************
 *
 * Name              : tmr_init
 *
 * Description       : initialize a timer object
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_init( tmr_t *tmr, fun_t *state );

/******************************************************************************
 *
 * Name              : tmr_startUntil
 *
 * Description       : start/restart one-shot timer until given timepoint and then launch the callback procedure
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   time            : timepoint value
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_startUntil( tmr_t *tmr, cnt_t time );

/******************************************************************************
 *
 * Name              : tmr_start
 *
 * Description       : start/restart periodic timer for given duration of time and then launch the callback procedure
 *                     do this periodically if period > 0
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_start( tmr_t *tmr, cnt_t delay, cnt_t period );

/******************************************************************************
 *
 * Name              : tmr_startFor
 *
 * Description       : start/restart one-shot timer for given duration of time and then launch the callback procedure
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_startFor( tmr_t *tmr, cnt_t delay ) { tmr_start(tmr, delay, 0); }

/******************************************************************************
 *
 * Name              : tmr_startPeriodic
 *
 * Description       : start/restart periodic timer for given duration of time and then launch the callback procedure
 *                     do this periodically
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   period          : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_startPeriodic( tmr_t *tmr, cnt_t period ) { tmr_start(tmr, period, period); }

/******************************************************************************
 *
 * Name              : tmr_startFrom
 *
 * Description       : start/restart periodic timer for given duration of time and then launch the callback procedure
 *                     do this periodically if period > 0
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   proc            : callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_startFrom( tmr_t *tmr, cnt_t delay, cnt_t period, fun_t *proc );

/******************************************************************************
 *
 * Name              : tmr_wait
 *
 * Description       : wait indefinitely until the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_wait( tmr_t *tmr );

/******************************************************************************
 *
 * Name              : tmr_take
 *
 * Description       : check if the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return
 *   E_SUCCESS       : timer object successfully finished countdown
 *   E_FAILURE       : timer object has not yet completed counting
 *
 ******************************************************************************/

unsigned tmr_take( tmr_t *tmr );

/******************************************************************************
 *
 * Name              : tmr_flip
 *
 * Description       : change callback procedure for current periodic timer (available in next period)
 *
 * Parameters
 *   proc            : new callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 * Note              : use only in timer's callback procedure
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_flip( fun_t *proc ) { tmr_this()->state = proc; }

/******************************************************************************
 *
 * Name              : tmr_delay
 *
 * Description       : change delay time for current timer
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     0:         current timer is stopped even if it was periodic
 *                     otherwise: current timer is restarted even if it was one-shot
 *
 * Return            : none
 *
 * Note              : use only in timer's callback procedure
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_delay( cnt_t delay ) { tmr_this()->delay = delay; }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Timer
 *
 * Description       : create and initialize a timer object
 *
 * Constructor parameters
 *   state           : callback procedure
 *
 ******************************************************************************/

struct Timer : public __tmr
{
	 explicit
	 Timer( void ):         __tmr _TMR_INIT(0) {}
#if OS_FUNCTIONAL
	 explicit
	 Timer( FUN_t _state ): __tmr _TMR_INIT(run_), fun_(_state) {}
	~Timer( void ) { assert(__tmr::id == ID_STOPPED); }
#else
	 explicit
	 Timer( FUN_t _state ): __tmr _TMR_INIT(_state) {}
	~Timer( void ) { assert(__tmr::id == ID_STOPPED); }
#endif
	void startUntil   ( cnt_t _time )                               {        tmr_startUntil   (this, _time);                   }
	void start        ( cnt_t _delay, cnt_t _period )               {        tmr_start        (this, _delay, _period);         }
	void startFor     ( cnt_t _delay )                              {        tmr_startFor     (this, _delay);                  }
	void startPeriodic( cnt_t _period )                             {        tmr_startPeriodic(this,         _period);         }
#if OS_FUNCTIONAL
	void startFrom    ( cnt_t _delay, cnt_t _period, FUN_t _state ) {        fun_ = _state;
	                                                                         tmr_startFrom    (this, _delay, _period, run_);   }
#else
	void startFrom    ( cnt_t _delay, cnt_t _period, FUN_t _state ) {        tmr_startFrom    (this, _delay, _period, _state); }
#endif
	void     wait     ( void )                                      {        tmr_wait         (this);                          }
	unsigned take     ( void )                                      { return tmr_take         (this);                          }

	bool     operator!( void )                                      { return __tmr::id == ID_STOPPED;                          }
#if OS_FUNCTIONAL
	static
	void     run_( void ) { ((Timer *) System.cur)->fun_(); }
	FUN_t    fun_;
#endif
};

/******************************************************************************
 *
 * Class             : startTimerUntil
 *
 * Description       : create and initialize a timer object
 *                     and start one-shot timer until given timepoint and then launch the callback procedure
 *
 * Constructor parameters
 *   time            : timepoint value
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerUntil : public Timer
{
	explicit
	startTimerUntil( const cnt_t _time ):               Timer()       { tmr_startUntil(this, _time); }
	explicit
	startTimerUntil( const cnt_t _time, FUN_t _state ): Timer(_state) { tmr_startUntil(this, _time); }
};

/******************************************************************************
 *
 * Class             : startTimer
 *
 * Description       : create and initialize a timer object
 *                     and start periodic timer for given duration of time and then launch the callback procedure
 *                     do this periodically
 *
 * Constructor parameters
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimer : public Timer
{
	explicit
	startTimer( const cnt_t _delay, const cnt_t _period ):               Timer()       { tmr_start(this, _delay, _period); }
	explicit
	startTimer( const cnt_t _delay, const cnt_t _period, FUN_t _state ): Timer(_state) { tmr_start(this, _delay, _period); }
};

/******************************************************************************
 *
 * Class             : startTimerFor
 *
 * Description       : create and initialize a timer object
 *                     and start one-shot timer for given duration of time and then launch the callback procedure
 *
 * Constructor parameters
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerFor : public Timer
{
	explicit
	startTimerFor( const cnt_t _delay ):               Timer()       { tmr_startFor(this, _delay); }
	explicit
	startTimerFor( const cnt_t _delay, FUN_t _state ): Timer(_state) { tmr_startFor(this, _delay); }
};

/******************************************************************************
 *
 * Class             : startTimerPeriodic
 *
 * Description       : create and initialize a timer object
 *                     and start periodic timer for given duration of time and then launch the callback procedure
 *                     do this periodically
 *
 * Constructor parameters
 *   period          : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerPeriodic : public Timer
{
	explicit
	startTimerPeriodic( const cnt_t _period ):               Timer()       { tmr_startPeriodic(this, _period); }
	explicit
	startTimerPeriodic( const cnt_t _period, FUN_t _state ): Timer(_state) { tmr_startPeriodic(this, _period); }
};

/******************************************************************************
 *
 * Namespace         : ThisTimer
 *
 * Description       : provide set of functions for Current Timer
 *
 ******************************************************************************/

namespace ThisTimer
{
#if OS_FUNCTIONAL
	static inline void flip ( FUN_t _state ) { ((Timer *) System.cur)->fun_ = _state;
	                                           tmr_flip (Timer::run_);                }
#else
	static inline void flip ( FUN_t _state ) { tmr_flip (_state);                     }
#endif
	static inline void delay( cnt_t _delay ) { tmr_delay(_delay);                     }
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_TMR_H
