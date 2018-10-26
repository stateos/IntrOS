/******************************************************************************

    @file    IntrOS: ostask.h
    @author  Rajmund Szymanski
    @date    26.10.2018
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

#ifndef __INTROS_TSK_H
#define __INTROS_TSK_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define STK_SIZE( size ) \
    ALIGNED_SIZE( size, stk_t )

#define STK_CROP( base, size ) \
         LIMITED( (size_t)base + size, stk_t )

/******************************************************************************
 *
 * Name              : task (thread)
 *
 ******************************************************************************/

struct __tsk
{
	hdr_t    hdr;   // timer / task header

	fun_t  * state; // task state (initial task function, doesn't have to be noreturn-type)
	cnt_t    start; // inherited from timer
	cnt_t    delay; // inherited from timer
	cnt_t    period;// inherited from timer

	struct {
	unsigned sigset;// pending signals
	act_t  * action;// signal handler
	struct {
	fun_t  * pc;
	cnt_t    delay;
	}        backup;
	}        sig;

	stk_t  * stack; // base of stack
	unsigned size;  // size of stack (in bytes)

	union  {
	ctx_t    reg;   // task context
	jmp_buf  buf;   // setjmp/longjmp buffer
	}        ctx;
};

/******************************************************************************
 *
 * Name              : _TSK_INIT
 *
 * Description       : create and initialize a task object
 *
 * Parameters
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Return            : task object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _TSK_INIT( _state, _stack, _size ) \
                    { _HDR_INIT(), _state, 0, 0, 0, { 0, 0, { 0, 0 } }, _stack, _size, { _CTX_INIT() } }

/******************************************************************************
 *
 * Name              : _TSK_CREATE
 *
 * Description       : create and initialize a task object
 *
 * Parameters
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Return            : pointer to task object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _TSK_CREATE( _state, _stack, _size ) \
          (tsk_t[]) { _TSK_INIT  ( _state, _stack, _size ) }
#endif

/******************************************************************************
 *
 * Name              : _TSK_STACK
 *
 * Description       : create task's private stack storage
 *
 * Parameters
 *   size            : size of task's private stack storage (in bytes)
 *
 * Return            : base of task's private stack storage
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _TSK_STACK( _size ) (stk_t[STK_SIZE(_size)]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : _VA_STK
 *
 * Description       : calculate stack size from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_STK( _size ) ((_size + 0) ? (_size + 0) : (OS_STACK_SIZE))

/******************************************************************************
 *
 * Name              : OS_WRK
 *
 * Description       : define and initialize complete work area for task object
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

#define             OS_WRK( tsk, state, size )                                \
                       stk_t tsk##__stk[STK_SIZE( size )];                     \
                       tsk_t tsk##__tsk = _TSK_INIT( state, tsk##__stk, size ); \
                       tsk_id tsk = & tsk##__tsk

/******************************************************************************
 *
 * Name              : OS_TSK
 *
 * Description       : define and initialize complete work area for task object with default stack size
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 ******************************************************************************/

#define             OS_TSK( tsk, state, ... ) \
                    OS_WRK( tsk, state, _VA_STK(__VA_ARGS__) )

/******************************************************************************
 *
 * Name              : OS_WRK_DEF
 *
 * Description       : define and initialize complete work area for task object
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

#define             OS_WRK_DEF( tsk, size )        \
                       void tsk##__fun( void );     \
                    OS_WRK( tsk, tsk##__fun, size ); \
                       void tsk##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TSK_DEF
 *
 * Description       : define and initialize complete work area for task object with default stack size
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 ******************************************************************************/

#define             OS_TSK_DEF( tsk, ... ) \
                    OS_WRK_DEF( tsk, _VA_STK(__VA_ARGS__) )

/******************************************************************************
 *
 * Name              : OS_WRK_START
 *
 * Description       : define, initialize and start complete work area for task object
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   size            : size of task private stack (in bytes)
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_WRK_START( tsk, size )                     \
                       void tsk##__fun( void );                    \
                    OS_WRK( tsk, tsk##__fun, size );                \
         __CONSTRUCTOR void tsk##__start( void ) { tsk_start(tsk); } \
                       void tsk##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TSK_START
 *
 * Description       : define, initialize and start complete work area for task object with default stack size
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TSK_START( tsk, ... ) \
                    OS_WRK_START( tsk, _VA_STK(__VA_ARGS__) )

/******************************************************************************
 *
 * Name              : static_WRK
 *
 * Description       : define and initialize static work area for task object
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

#define         static_WRK( tsk, state, size )                                \
                static stk_t tsk##__stk[STK_SIZE( size )];                     \
                static tsk_t tsk##__tsk = _TSK_INIT( state, tsk##__stk, size ); \
                static tsk_id tsk = & tsk##__tsk

/******************************************************************************
 *
 * Name              : static_TSK
 *
 * Description       : define and initialize static work area for task object with default stack size
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 ******************************************************************************/

#define         static_TSK( tsk, state, ... ) \
                static_WRK( tsk, state, _VA_STK(__VA_ARGS__) )

/******************************************************************************
 *
 * Name              : static_WRK_DEF
 *
 * Description       : define and initialize static work area for task object
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   size            : size of task private stack (in bytes)
 *
 ******************************************************************************/

#define         static_WRK_DEF( tsk, size )        \
                static void tsk##__fun( void );     \
                static_WRK( tsk, tsk##__fun, size ); \
                static void tsk##__fun( void )

/******************************************************************************
 *
 * Name              : static_TSK_DEF
 *
 * Description       : define and initialize static work area for task object with default stack size
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 ******************************************************************************/

#define         static_TSK_DEF( tsk, ... ) \
                static_WRK_DEF( tsk, _VA_STK(__VA_ARGS__) )

/******************************************************************************
 *
 * Name              : static_WRK_START
 *
 * Description       : define, initialize and start static work area for task object
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   size            : size of task private stack (in bytes)
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_WRK_START( tsk, size )                     \
                static void tsk##__fun( void );                    \
                static_WRK( tsk, tsk##__fun, size );                \
  __CONSTRUCTOR static void tsk##__start( void ) { tsk_start(tsk); } \
                static void tsk##__fun( void )

/******************************************************************************
 *
 * Name              : static_TSK_START
 *
 * Description       : define, initialize and start static work area for task object with default stack size
 *                     task state (function body) must be defined immediately below
 *
 * Parameters
 *   tsk             : name of a pointer to task object
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TSK_START( tsk, ... ) \
                static_WRK_START( tsk, _VA_STK(__VA_ARGS__) )

/******************************************************************************
 *
 * Name              : WRK_INIT
 *
 * Description       : create and initialize complete work area for task object
 *
 * Parameters
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 * Return            : task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                WRK_INIT( state, size ) \
                      _TSK_INIT( state, _TSK_STACK( size ), size )
#endif

/******************************************************************************
 *
 * Name              : WRK_CREATE
 * Alias             : WRK_NEW
 *
 * Description       : create and initialize complete work area for task object
 *
 * Parameters
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : size of task private stack (in bytes)
 *
 * Return            : pointer to task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                WRK_CREATE( state, size ) \
           (tsk_t[]) { WRK_INIT  ( state, size ) }
#define                WRK_NEW \
                       WRK_CREATE
#endif

/******************************************************************************
 *
 * Name              : TSK_INIT
 *
 * Description       : create and initialize complete work area for task object with default stack size
 *
 * Parameters
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 * Return            : task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TSK_INIT( state, ... ) \
                       WRK_INIT( state, _VA_STK(__VA_ARGS__) )
#endif

/******************************************************************************
 *
 * Name              : TSK_CREATE
 * Alias             : TSK_NEW
 *
 * Description       : create and initialize complete work area for task object with default stack size
 *
 * Parameters
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   size            : (optional) size of task private stack (in bytes); default: OS_STACK_SIZE
 *
 * Return            : pointer to task object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TSK_CREATE( state, ... ) \
                       WRK_CREATE( state, _VA_STK(__VA_ARGS__) )
#define                TSK_NEW \
                       TSK_CREATE
#endif

/******************************************************************************
 *
 * Name              : tsk_this
 * Alias             : cur_task
 *
 * Description       : return current task object
 *
 * Parameters        : none
 *
 * Return            : current task object
 *
 ******************************************************************************/

__STATIC_INLINE
tsk_t *tsk_this( void ) { return System.cur; }

__STATIC_INLINE
tsk_t *cur_task( void ) { return System.cur; }

/******************************************************************************
 *
 * Name              : tsk_init
 *
 * Description       : initialize complete work area for task object and start the task
 *
 * Parameters
 *   tsk             : pointer to task object
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *   stack           : base of task's private stack storage
 *   size            : size of task private stack (in bytes)
 *
 * Return            : task object
 *
 ******************************************************************************/

void tsk_init( tsk_t *tsk, fun_t *state, stk_t *stack, unsigned size );

/******************************************************************************
 *
 * Name              : tsk_start
 *
 * Description       : start previously defined/created/stopped task object
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_start( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_startFrom
 *
 * Description       : start previously defined/created/stopped task object
 *
 * Parameters
 *   tsk             : pointer to task object
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_startFrom( tsk_t *tsk, fun_t *state );

/******************************************************************************
 *
 * Name              : tsk_stop
 *
 * Description       : stop execution of current task
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__NO_RETURN
void tsk_stop( void );

/******************************************************************************
 *
 * Name              : tsk_reset
 * Alias             : tsk_kill
 *
 * Description       : stop execution of given task
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_reset( tsk_t *tsk );

__STATIC_INLINE
void tsk_kill( tsk_t *tsk ) { tsk_reset(tsk); }

/******************************************************************************
 *
 * Name              : cur_reset
 * Alias             : cur_kill
 *
 * Description       : stop execution of current task
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void cur_reset( void ) { tsk_reset(System.cur); }

__STATIC_INLINE
void cur_kill( void ) { cur_reset(); }

/******************************************************************************
 *
 * Name              : tsk_join
 *
 * Description       : delay execution of current task until termination of given task
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_join( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_yield
 * Alias             : tsk_pass
 *
 * Description       : yield system control to the next task
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void tsk_yield( void ) { core_ctx_switch(); }

__STATIC_INLINE
void tsk_pass ( void ) { core_ctx_switch(); }

/******************************************************************************
 *
 * Name              : tsk_flip
 *
 * Description       : restart current task with the new state (task function)
 *
 * Parameters
 *   proc            : new task state (task function)
 *
 * Return            : none
 *
 ******************************************************************************/

__NO_RETURN
void tsk_flip( fun_t *state );

/******************************************************************************
 *
 * Name              : tsk_sleepFor
 * Alias             : tsk_delay
 *
 * Description       : delay execution of current task for given duration of time
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)
 *                     IMMEDIATE: don't delay execution of current task
 *                     INFINITE:  delay indefinitely execution of current task
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_sleepFor( cnt_t delay );

__STATIC_INLINE
void tsk_delay( cnt_t delay ) { tsk_sleepFor(delay); }

/******************************************************************************
 *
 * Name              : tsk_sleepNext
 *
 * Description       : delay execution of current task for given duration of time
 *                     from the end of the previous countdown
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)
 *                     IMMEDIATE: don't delay execution of current task
 *                     INFINITE:  delay indefinitely execution of current task
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_sleepNext( cnt_t delay );

/******************************************************************************
 *
 * Name              : tsk_sleepUntil
 *
 * Description       : delay execution of current task until given timepoint
 *
 * Parameters
 *   time            : timepoint value
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_sleepUntil( cnt_t time );

/******************************************************************************
 *
 * Name              : tsk_sleep
 *
 * Description       : delay indefinitely execution of current task
 *                     execution of the task can be resumed
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void tsk_sleep( void ) { tsk_sleepFor(INFINITE); }

/******************************************************************************
 *
 * Name              : tsk_suspend
 *
 * Description       : delay indefinitely execution of given task
 *                     execution of the task can be resumed
 *
 * Parameters
 *   tsk             : pointer to task object
 *
 * Return
 *   E_SUCCESS       : task was successfully suspended
 *   E_FAILURE       : task cannot be suspended
 *
 ******************************************************************************/

unsigned tsk_suspend( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : cur_suspend
 *
 * Description       : delay indefinitely execution of given task
 *                     execution of the task can be resumed
 *
 * Parameters        : none
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void cur_suspend( void ) { tsk_suspend(System.cur); }

/******************************************************************************
 *
 * Name              : tsk_resume
 *
 * Description       : resume execution of given suspended task
 *                     only suspended or indefinitely delayed tasks can be resumed
 *
 * Parameters
 *   tsk             : pointer to delayed task object
 *
 * Return
 *   E_SUCCESS       : task was successfully resumed
 *   E_FAILURE       : task cannot be resumed
 *
 ******************************************************************************/

unsigned tsk_resume( tsk_t *tsk );

/******************************************************************************
 *
 * Name              : tsk_give
 * Alias             : tsk_signal
 *
 * Description       : send given signal to the task
 *
 * Parameters
 *   tsk             : pointer to the task object
 *   signo           : signal number
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_give( tsk_t *tsk, unsigned signo );

__STATIC_INLINE
void tsk_signal( tsk_t *tsk, unsigned signo ) { tsk_give(tsk, signo); }

/******************************************************************************
 *
 * Name              : cur_give
 * Alias             : cur_signal
 *
 * Description       : send given signal to the current task
 *
 * Parameters
 *   signo           : signal number
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void cur_give( unsigned signo ) { tsk_give(System.cur, signo); }

__STATIC_INLINE
void cur_signal( unsigned signo ) { cur_give(signo); }

/******************************************************************************
 *
 * Name              : tsk_action
 *
 * Description       : set given function as a signal handler
 *
 * Parameters
 *   tsk             : pointer to the task object
 *   action          : signal handler
 *
 * Return            : none
 *
 ******************************************************************************/

void tsk_action( tsk_t *tsk, act_t *action );

/******************************************************************************
 *
 * Name              : cur_action
 *
 * Description       : set given function as a signal handler for current task
 *
 * Parameters
 *   signo           : signal number
 *   action          : signal handler
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void cur_action( act_t *action ) { tsk_action(System.cur, action); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : staticTaskT<>
 *
 * Description       : create and initialize complete work area for static task object
 *
 * Constructor parameters
 *   size            : size of task private stack (in bytes)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

template<unsigned size_ = OS_STACK_SIZE>
struct staticTaskT : public __tsk
{
	 staticTaskT( fun_t *_state ): __tsk _TSK_INIT(_state, stack_, size_) {}
	~staticTaskT( void ) { assert(__tsk::hdr.id == ID_STOPPED); }

	void     start    ( void )             {        tsk_start    (this);          }
	void     startFrom( fun_t  * _state )  {        tsk_startFrom(this, _state);  }
	void     join     ( void )             {        tsk_join     (this);          }
	void     reset    ( void )             {        tsk_reset    (this);          }
	void     kill     ( void )             {        tsk_kill     (this);          }
	unsigned suspend  ( void )             { return tsk_suspend  (this);          }
	unsigned resume   ( void )             { return tsk_resume   (this);          }
	void     give     ( unsigned _signo )  {        tsk_give     (this, _signo);  }
	void     signal   ( unsigned _signo )  {        tsk_signal   (this, _signo);  }
	void     action   ( act_t  * _action ) {        tsk_action   (this, _action); }
	bool     operator!( void )             { return __tsk::hdr.id == ID_STOPPED;  }

	private:
	stk_t stack_[STK_SIZE(size_)];
};

/* -------------------------------------------------------------------------- */

typedef staticTaskT<OS_STACK_SIZE> staticTask;

/******************************************************************************
 *
 * Class             : TaskT<>
 *
 * Description       : create and initialize complete work area for task object
 *
 * Constructor parameters
 *   size            : size of task private stack (in bytes)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

template<unsigned size_ = OS_STACK_SIZE>
struct TaskT : public staticTaskT<size_>
{
#if OS_FUNCTIONAL
	TaskT( FUN_t _state ): staticTaskT<size_>(fun_), Fun_(_state) {}

	void  startFrom( FUN_t _state )  { Fun_ =   _state; tsk_startFrom(this, fun_); }
	void  action   ( ACT_t _action ) { Act_ = &_action; tsk_action   (this, act_); }

	static
	void  fun_( void )          {    ((TaskT<>*)System.cur)->Fun_();       }
	FUN_t  Fun_;
	static
	void  act_( unsigned _sig ) { (*(((TaskT<>*)System.cur)->Act_))(_sig); }
	ACT_t *Act_;
#else
	TaskT( FUN_t _state ): staticTaskT<size_>(_state) {}
#endif
};

/* -------------------------------------------------------------------------- */

typedef TaskT<OS_STACK_SIZE> Task;

/******************************************************************************
 *
 * Class             : startTaskT<>
 *
 * Description       : create and initialize complete work area for autorun task object
 *
 * Constructor parameters
 *   size            : size of task private stack (in bytes)
 *   state           : task state (initial task function) doesn't have to be noreturn-type
 *                     it will be executed into an infinite system-implemented loop
 *
 ******************************************************************************/

template<unsigned size_ = OS_STACK_SIZE>
struct startTaskT : public TaskT<size_>
{
	startTaskT( FUN_t _state ): TaskT<size_>(_state) { tsk_start(this); }
};

/* -------------------------------------------------------------------------- */

typedef startTaskT<OS_STACK_SIZE> startTask;

/******************************************************************************
 *
 * Namespace         : ThisTask
 *
 * Description       : provide set of functions for current task
 *
 ******************************************************************************/

namespace ThisTask
{
	static inline void     stop      ( void )             { tsk_stop      ();                      }
	static inline void     reset     ( void )             { cur_reset     ();                      }
	static inline void     kill      ( void )             { cur_kill      ();                      }
	static inline void     yield     ( void )             { tsk_yield     ();                      }
	static inline void     pass      ( void )             { tsk_pass      ();                      }
#if OS_FUNCTIONAL
	static inline void     flip      ( FUN_t    _state )  { ((TaskT<>*)System.cur)->Fun_ =   _state;
	                                                        tsk_flip      (TaskT<>::fun_);         }
#else
	static inline void     flip      ( FUN_t    _state )  { tsk_flip      (_state);                }
#endif
	static inline void     sleepFor  ( cnt_t    _delay )  { tsk_sleepFor  (_delay);                }
	static inline void     sleepNext ( cnt_t    _delay )  { tsk_sleepNext (_delay);                }
	static inline void     sleepUntil( cnt_t    _time )   { tsk_sleepUntil(_time);                 }
	static inline void     sleep     ( void )             { tsk_sleep     ();                      }
	static inline void     delay     ( cnt_t    _delay )  { tsk_delay     (_delay);                }
	static inline void     suspend   ( void )             { cur_suspend   ();                      }
	static inline void     give      ( unsigned _signo )  { cur_give      (_signo);                }
	static inline void     signal    ( unsigned _signo )  { cur_signal    (_signo);                }
#if OS_FUNCTIONAL
	static inline void     action    ( ACT_t    _action ) { ((TaskT<>*)System.cur)->Act_ = &_action;
	                                                        cur_action    (TaskT<>::act_);         }
#else
	static inline void     action    ( ACT_t    _action ) { cur_action    (_action);               }
#endif
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_TSK_H
