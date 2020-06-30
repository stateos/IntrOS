/******************************************************************************

    @file    IntrOS: ostimer.h
    @author  Rajmund Szymanski
    @date    30.06.2020
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

#ifndef __INTROS_TMR_H
#define __INTROS_TMR_H

#include "oskernel.h"
#include "osclock.h"

/******************************************************************************
 *
 * Name              : timer
 *
 ******************************************************************************/

struct __tmr
{
	hdr_t    hdr;   // timer / task header

	fun_t  * state; // callback procedure
	cnt_t    start;
	cnt_t    delay;
	cnt_t    period;

	unsigned signal;
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _TMR_INIT
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     NULL: no callback
 *
 * Return            : timer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _TMR_INIT( _state ) \
                    { _HDR_INIT(), _state, 0, 0, 0, 0 }

/******************************************************************************
 *
 * Name              : OS_TMR
 *
 * Description       : define and initialize a timer object
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   state           : callback procedure
 *                     NULL: no callback
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

#ifdef __CONSTRUCTOR
#define             OS_TMR_START( tmr, delay, period )                          \
                       void tmr##__fun( void );                                  \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                \
                       tmr_id tmr = & tmr##__tmr;                                  \
         __CONSTRUCTOR void tmr##__start( void ) { tmr_start(tmr, delay, period); } \
                       void tmr##__fun( void )
#endif

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

#ifdef __CONSTRUCTOR
#define             OS_TMR_UNTIL( tmr, time )                               \
                       void tmr##__fun( void );                              \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );            \
                       tmr_id tmr = & tmr##__tmr;                              \
         __CONSTRUCTOR void tmr##__start( void ) { tmr_startUntil(tmr, time); } \
                       void tmr##__fun( void )
#endif

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
 *                     NULL: no callback
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

#ifdef __CONSTRUCTOR
#define         static_TMR_START( tmr, delay, period )                          \
                static void tmr##__fun( void );                                  \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                \
                static tmr_id tmr = & tmr##__tmr;                                  \
  __CONSTRUCTOR static void tmr##__start( void ) { tmr_start(tmr, delay, period); } \
                static void tmr##__fun( void )
#endif

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

#ifdef __CONSTRUCTOR
#define         static_TMR_UNTIL( tmr, time )                               \
                static void tmr##__fun( void );                              \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );            \
                static tmr_id tmr = & tmr##__tmr;                              \
  __CONSTRUCTOR static void tmr##__start( void ) { tmr_startUntil(tmr, time); } \
                static  void tmr##__fun( void )
#endif

/******************************************************************************
 *
 * Name              : TMR_INIT
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     NULL: no callback
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
 *                     NULL: no callback
 *
 * Return            : pointer to timer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TMR_CREATE( state ) \
           (tmr_t[]) { TMR_INIT  ( state ) }
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
 * Note              : use only in timer's callback procedure
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
 *                     NULL: no callback
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_init( tmr_t *tmr, fun_t *state );

/******************************************************************************
 *
 * Name              : tmr_start
 *
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
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
 * Description       : start/restart one-shot timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
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
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
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
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
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
 *                     NULL: no callback
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_startFrom( tmr_t *tmr, cnt_t delay, cnt_t period, fun_t *proc );

/******************************************************************************
 *
 * Name              : tmr_startNext
 *
 * Description       : restart the timer for given duration of time from the end of the previous countdown
 *                     when the timer has finished the countdown, the callback procedure is launched
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

void tmr_startNext( tmr_t *tmr, cnt_t delay );

/******************************************************************************
 *
 * Name              : tmr_startUntil
 *
 * Description       : start/restart one-shot timer until given timepoint
 *                     when the timer has finished the countdown, the callback procedure is launched
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
 * Name              : tmr_take
 * Alias             : tmr_tryWait
 *
 * Description       : check if the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return
 *   SUCCESS         : timer object successfully finished countdown
 *   FAILURE         : timer object has not yet completed counting
 *
 ******************************************************************************/

unsigned tmr_take( tmr_t *tmr );

__STATIC_INLINE
unsigned tmr_tryWait( tmr_t *tmr ) { return tmr_take(tmr); }

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
 * Name              : tmr_flip
 *
 * Description       : change callback procedure for current periodic timer (available in next period)
 *
 * Parameters
 *   proc            : new callback procedure
 *                     NULL: no callback
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
 * Class             : baseTimer
 *
 * Description       : create and initialize base class for timer objects
 *
 * Constructor parameters
 *   state           : callback procedure
 *                     nullptr: no callback
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseTimer : public __tmr
{
	baseTimer( void ):           __tmr _TMR_INIT(nullptr) {}
#if __cplusplus >= 201402
	template<class F>
	baseTimer( F&&     _state ): __tmr _TMR_INIT(fun_), fun{_state} {}
#else
	baseTimer( fun_t * _state ): __tmr _TMR_INIT(_state) {}
#endif

	template<typename T>
	void     start        ( const T _delay, const T _period )                 {        tmr_start        (this, Clock::count(_delay), Clock::count(_period)); }
	template<typename T>
	void     startFor     ( const T _delay )                                  {        tmr_startFor     (this, Clock::count(_delay)); }
	template<typename T>
	void     startPeriodic( const T _period )                                 {        tmr_startPeriodic(this, Clock::count(_period)); }
	template<typename T>
	void     startNext    ( const T _delay )                                  {        tmr_startNext    (this, Clock::count(_delay)); }
	template<typename T>
	void     startUntil   ( const T _time )                                   {        tmr_startUntil   (this, Clock::until(_time)); }
#if __cplusplus >= 201402
	template<typename T>
	void     startFrom    ( const T _delay, const T _period, std::nullptr_t ) {        tmr_startFrom    (this, Clock::count(_delay), Clock::count(_period), nullptr); }
	template<typename T, class F>
	void     startFrom    ( const T _delay, const T _period, F&&     _state ) {        new (&fun) Fun_t(_state);
	                                                                                   tmr_startFrom    (this, Clock::count(_delay), Clock::count(_period), fun_); }
#else
	template<typename T>
	void     startFrom    ( const T _delay, const T _period, fun_t * _state ) {        tmr_startFrom    (this, Clock::count(_delay), Clock::count(_period), _state); }
#endif
	unsigned take         ( void )                                            { return tmr_take         (this); }
	unsigned tryWait      ( void )                                            { return tmr_tryWait      (this); }
	void     wait         ( void )                                            {        tmr_wait         (this); }
	explicit
	operator bool         () const                                            { return __tmr::hdr.id != ID_STOPPED; }

	template<class T = baseTimer> static
	T *      current      ( void )                                            { return static_cast<T *>(tmr_this()); }

#if __cplusplus >= 201402
	static
	void     fun_         ( void )                                            {        current()->fun(); }
	Fun_t    fun;
#endif

/******************************************************************************
 *
 * Class             : [base]Timer::Current
 *
 * Description       : provide set of functions for current timer
 *
 ******************************************************************************/

	struct Current
	{
#if __cplusplus >= 201402
		static
		void flip ( std::nullptr_t )           { tmr_flip (nullptr); }
		template<class F> static
		void flip ( F&& _state )               { new (&current()->fun) Fun_t(_state);
		                                         tmr_flip (fun_); }
		template<typename F, typename... A> static
		void flip ( F&& _state, A&&... _args ) { flip(std::bind(std::forward<F>(_state), std::forward<A>(_args)...)); }
#else
		static
		void flip ( fun_t * _state )           { tmr_flip (_state); }
#endif
		template<typename T> static
		void delay( const T _delay )           { tmr_delay(Clock::count(_delay)); }
	};
};

using ThisTimer = baseTimer::Current;
using Self      = baseTimer::Current;

/******************************************************************************
 *
 * Class             : Timer
 *
 * Description       : create and initialize a timer object
 *
 * Constructor parameters
 *   state           : callback procedure
 *                     none / nullptr: no callback
 *
 ******************************************************************************/

struct Timer : public baseTimer
{
	Timer( void ):                     baseTimer{} {}
	template<class F>
	Timer( F&& _state ):               baseTimer{_state} {}
#if __cplusplus >= 201402
	Timer( std::nullptr_t ):           baseTimer{} {}
	template<typename F, typename... A>
	Timer( F&& _state, A&&... _args ): baseTimer{std::bind(std::forward<F>(_state), std::forward<A>(_args)...)} {}
#endif

	Timer( Timer&& ) = default;
	Timer( const Timer& ) = delete;
	Timer& operator=( Timer&& ) = delete;
	Timer& operator=( const Timer& ) = delete;

	~Timer( void ) { assert(__tmr::hdr.id == ID_STOPPED); }

/******************************************************************************
 *
 * Name              : Timer::Make
 *
 * Description       : create and initialize timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     none / nullptr: no callback
 *   args            : arguments for callback procedure
 *
 * Return            : Timer object
 *
 ******************************************************************************/

	static
	Timer Make( void )
	{
		return {};
	}

	template<class F> static
	Timer Make( F&& _state )
	{
		return { _state };
	}

#if __cplusplus >= 201402
	static
	Timer Make( std::nullptr_t )
	{
		return Make();
	}

	template<typename F, typename... A> static
	Timer Make( F&& _state, A&&... _args )
	{
		return Make(std::bind(std::forward<F>(_state), std::forward<A>(_args)...));
	}
#endif

/******************************************************************************
 *
 * Name              : Timer::Start
 *
 * Description       : create and initialize timer object
 *                     and start periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *                     none / nullptr: no callback
 *   args            : arguments for callback procedure
 *
 * Return            : Timer object
 *
 ******************************************************************************/

	template<typename T> static
	Timer Start( const T _delay, const T _period )
	{
		Timer tmr {};
		tmr.start(Clock::count(_delay), Clock::count(_period));
		return tmr;
	}

	template<typename T, class F> static
	Timer Start( const T _delay, const T _period, F&& _state )
	{
		Timer tmr { _state };
		tmr.start(Clock::count(_delay), Clock::count(_period));
		return tmr;
	}

#if __cplusplus >= 201402
	template<typename T> static
	Timer Start( const T _delay, const T _period, std::nullptr_t )
	{
		return Start(_delay, _period);
	}

	template<typename T, typename F, typename... A> static
	Timer Start( const T _delay, const T _period, F&& _state, A&&... _args )
	{
		return Start(_delay, _period, std::bind(std::forward<F>(_state), std::forward<A>(_args)...));
	}
#endif

/******************************************************************************
 *
 * Name              : Timer::StartFor
 *
 * Description       : create and initialize timer object
 *                     and start one-shot timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *                     none / nullptr: no callback
 *   args            : arguments for callback procedure
 *
 * Return            : Timer object
 *
 ******************************************************************************/

	template<typename T> static
	Timer StartFor( const T _delay )
	{
		Timer tmr {};
		tmr.startFor(Clock::count(_delay));
		return tmr;
	}

	template<typename T, class F> static
	Timer StartFor( const T _delay, F&& _state )
	{
		Timer tmr { _state };
		tmr.startFor(Clock::count(_delay));
		return tmr;
	}

#if __cplusplus >= 201402
	template<typename T> static
	Timer StartFor( const T _delay, std::nullptr_t )
	{
		return StartFor(_delay);
	}

	template<typename T, typename F, typename... A> static
	Timer StartFor( const T _delay, F&& _state, A&&... _args )
	{
		return StartFor(_delay, std::bind(std::forward<F>(_state), std::forward<A>(_args)...));
	}
#endif

/******************************************************************************
 *
 * Name              : Timer::StartPeriodic
 *
 * Description       : create and initialize timer object
 *                     and start periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically
 *
 * Parameters
 *   period          : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *                     none / nullptr: no callback
 *   args            : arguments for callback procedure
 *
 * Return            : Timer object
 *
 ******************************************************************************/

	template<typename T> static
	Timer StartPeriodic( const T _period )
	{
		Timer tmr {};
		tmr.startPeriodic(Clock::count(_period));
		return tmr;
	}

	template<typename T, class F> static
	Timer StartPeriodic( const T _period, F&& _state )
	{
		Timer tmr { _state };
		tmr.startPeriodic(Clock::count(_period));
		return tmr;
	}

#if __cplusplus >= 201402
	template<typename T> static
	Timer StartPeriodic( const T _period, std::nullptr_t )
	{
		return StartPeriodic(_period);
	}

	template<typename T, typename F, typename... A> static
	Timer StartPeriodic( const T _period, F&& _state, A&&... _args )
	{
		return StartPeriodic(_period, std::bind(std::forward<F>(_state), std::forward<A>(_args)...));
	}
#endif

/******************************************************************************
 *
 * Name              : Timer::StartUntil
 *
 * Description       : create and initialize timer object
 *                     and start one-shot timer until given timepoint
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Parameters
 *   time            : timepoint value
 *   state           : callback procedure
 *                     none / nullptr: no callback
 *   args            : arguments for callback procedure
 *
 * Return            : Timer object
 *
 ******************************************************************************/

	template<typename T> static
	Timer StartUntil( const T _time )
	{
		Timer tmr {};
		tmr.startUntil(Clock::until(_time));
		return tmr;
	}

	template<typename T, class F> static
	Timer StartUntil( const T _time, F&& _state )
	{
		Timer tmr { _state };
		tmr.startUntil(Clock::until(_time));
		return tmr;
	}

#if __cplusplus >= 201402
	template<typename T> static
	Timer StartUntil( const T _time, std::nullptr_t )
	{
		return StartUntil(_time);
	}

	template<typename T, typename F, typename... A> static
	Timer StartUntil( const T _time, F&& _state, A&&... _args )
	{
		return StartUntil(_time, std::bind(std::forward<F>(_state), std::forward<A>(_args)...));
	}
#endif
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_TMR_H
