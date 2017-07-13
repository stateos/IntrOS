/******************************************************************************

    @file    IntrOS: os_tsk.h
    @author  Rajmund Szymanski
    @date    12.07.2017
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

#ifndef __INTROS_TSK_H
#define __INTROS_TSK_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : task (thread)                                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct __tsk
{
	unsigned id;    // inherited from timer
	tsk_t  * prev;  // inherited from timer
	tsk_t  * next;  // inherited from timer
	unsigned event; // wakeup event

	fun_t  * state; // inherited from timer
	uint32_t start; // inherited from timer
	uint32_t delay; // inherited from timer
	void   * top;   // top of stack

	union  {
	ctx_t    reg;   // task context
	jmp_buf  buf;   // setjmp/longjmp buffer
	}        ctx;
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TSK_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a task object                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   top             : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _TSK_INIT( _state, _top ) { 0, 0, 0, 0, _state, 0, 0, _top, { _CTX_INIT() } }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TSK_CREATE                                                                                    *
 *                                                                                                                    *
 * Description       : create and initilize a task object                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   top             : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Return            : pointer to task object                                                                         *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _TSK_CREATE( _state, _top ) & (tsk_t) _TSK_INIT( _state, _top )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TSK_STACK                                                                                     *
 *                                                                                                                    *
 * Description       : create task's private stack storage                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   size            : size of task's private stack storage (in bytes)                                                *
 *                                                                                                                    *
 * Return            : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _TSK_STACK( _size ) ( stk_t[ASIZE( _size )] ){ 0 } + ASIZE( _size )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_WRK                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_WRK( tsk, state, size )                                          \
                       stk_t tsk##__stk[ASIZE( size )];                                  \
                       tsk_t tsk##__tsk = _TSK_INIT( state, tsk##__stk + ASIZE( size ) ); \
                       tsk_id tsk = & tsk##__tsk

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TSK                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TSK( tsk, state ) \
                    OS_WRK( tsk, state, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_WRK_DEF                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task object                                        *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_WRK_DEF( tsk, size )        \
                       void tsk##__fun( void );     \
                    OS_WRK( tsk, tsk##__fun, size ); \
                       void tsk##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TSK_DEF                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TSK_DEF( tsk ) \
                    OS_WRK_DEF( tsk, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_WRK                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_WRK( tsk, state, size )                                          \
                static stk_t tsk##__stk[ASIZE( size )];                                  \
                static tsk_t tsk##__tsk = _TSK_INIT( state, tsk##__stk + ASIZE( size ) ); \
                static tsk_id tsk = & tsk##__tsk

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TSK                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TSK( tsk, state ) \
                static_WRK( tsk, state, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_WRK_DEF                                                                                 *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object                                          *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_WRK_DEF( tsk, size )        \
                static void tsk##__fun( void );     \
                static_WRK( tsk, tsk##__fun, size ); \
                static void tsk##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TSK_DEF                                                                                 *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object with stack size defined by OS_STACK_SIZE *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TSK_DEF( tsk ) \
                static_WRK_DEF( tsk, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : WRK_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                WRK_INIT( state, size ) \
                      _TSK_INIT( state, _TSK_STACK( size ) )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : WRK_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 * Return            : pointer to task object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                WRK_CREATE( state, size ) \
             & (tsk_t) WRK_INIT( state, size )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TSK_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TSK_INIT( state ) \
                       WRK_INIT( state, OS_STACK_SIZE )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TSK_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : pointer to task object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TSK_CREATE( state ) \
                       WRK_CREATE( state, OS_STACK_SIZE )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_start                                                                                      *
 *                                                                                                                    *
 * Description       : start previously defined/created/stopped task object                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_start( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_startFrom                                                                                  *
 *                                                                                                                    *
 * Description       : start previously defined/created/stopped task object                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_startFrom( tsk_t *tsk, fun_t *state );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_stop                                                                                       *
 *                                                                                                                    *
 * Description       : stop current task and remove it from READY queue                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to current task object                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

__NO_RETURN
void tsk_stop( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_join                                                                                       *
 *                                                                                                                    *
 * Description       : delay execution of current task until termination of given task                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_join( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_yield                                                                                      *
 *                                                                                                                    *
 * Description       : yield system control to the next task                                                          *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void tsk_yield( void ) { core_ctx_switch(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_pass                                                                                       *
 *                                                                                                                    *
 * Description       : the same as tsk_yield (force context switch)                                                   *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void tsk_pass( void ) { core_ctx_switch(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_flip                                                                                       *
 *                                                                                                                    *
 * Description       : restart current task with the new state (task function)                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   proc            : new task state (task function)                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

__NO_RETURN
void tsk_flip( fun_t *state );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_wait                                                                                       *
 *                                                                                                                    *
 * Description       : delay indefinitly execution of current task and wait for flags or message                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flags           : all flags to wait                                                                              *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_wait( unsigned flags );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_give                                                                                       *
 *                                                                                                                    *
 * Description       : set given flags in waiting task (tsk_wait)                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to delayed task object                                                                 *
 *   flags           : flags or message transfered to the task                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_give( tsk_t *tsk, unsigned flags );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleepUntil                                                                                 *
 *                                                                                                                    *
 * Description       : delay execution of current task until given timepoint                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : task object successfully finished countdown                                                    *
 *   E_FAILURE       : task was resumed                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_sleepUntil( uint32_t time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleepFor                                                                                   *
 *                                                                                                                    *
 * Description       : delay execution of current task for given duration of time                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)                  *
 *                     IMMEDIATE: don't delay execution of current task                                               *
 *                     INFINITE:  delay indefinitly execution of current task                                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : task object successfully finished countdown                                                    *
 *   E_FAILURE       : task was resumed                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_sleepFor( uint32_t delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleep                                                                                      *
 *                                                                                                                    *
 * Description       : delay indefinitly execution of current task                                                    *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_FAILURE       : task was resumed                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_sleep( void ) { return tsk_sleepFor(INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_delay                                                                                      *
 *                                                                                                                    *
 * Description       : the same as tsk_sleepFor, delay execution of current task for given duration of time           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)                  *
 *                     IMMEDIATE: don't delay execution of current task                                               *
 *                     INFINITE:  delay indefinitly execution of current task                                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : task object successfully finished countdown                                                    *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_delay( uint32_t delay ) { return tsk_sleepFor(delay); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_suspend                                                                                    *
 *                                                                                                                    *
 * Description       : delay indefinitly execution of given task                                                      *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : task was successfully suspended                                                                *
 *   E_FAILURE       : task can not be suspended                                                                      *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_suspend( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_resume                                                                                     *
 *                                                                                                                    *
 * Description       : resume execution of given delayed task                                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to delayed task object                                                                 *
 *   event           : the value at which the given task is woken up                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : task was successfully resumed                                                                  *
 *   E_FAILURE       : task can not be resumed                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_resume( tsk_t *tsk );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : baseTask                                                                                       *
 *                                                                                                                    *
 * Description       : create task object                                                                             *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   stack           : initial value of task private stack pointer                                                    *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct baseTask : public __tsk
{
	 explicit
	 baseTask( FUN_t _state, stk_t *_stack ): __tsk _TSK_INIT((fun_t *) run, _stack), _start(_state) {}
	~baseTask( void ) { assert(id == ID_STOPPED); }

	void     join     ( void )         {        tsk_join     (this);                }
	void     start    ( void )         {        tsk_start    (this);                }
	void     startFrom( FUN_t _state ) {        _start = _state;
	                                            tsk_startFrom(this, (fun_t *) run); }
	unsigned suspend  ( void )         { return tsk_resume   (this);                }
	unsigned resume   ( void )         { return tsk_resume   (this);                }

	bool     operator!( void )         { return __tsk::id == ID_STOPPED;            }

	static
	void     run( baseTask &tsk ) { tsk._start(); }
	FUN_t    _start;
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : TaskT<>                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   size            : size of task private stack (in bytes)                                                          *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _size>
struct TaskT : public baseTask
{
	explicit
	TaskT( FUN_t _state ): baseTask(_state, _stack+ASIZE(_size)) {}

	private:
	stk_t _stack[ASIZE(_size)];
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Task                                                                                           *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object with default stack size                *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Task: public TaskT<OS_STACK_SIZE>
{
	explicit
	Task( FUN_t _state ): TaskT<OS_STACK_SIZE>(_state) {}
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTaskT<>                                                                                   *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                     and start task object                                                                          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   size            : size of task private stack (in bytes)                                                          *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _size>
struct startTaskT : public TaskT<_size>
{
	explicit
	startTaskT( FUN_t _state ): TaskT<_size>(_state) { tsk_start(this); }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTask                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object with default stack size                *
 *                     and start task object                                                                          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTask : public startTaskT<OS_STACK_SIZE>
{
	explicit
	startTask( FUN_t _state ): startTaskT<OS_STACK_SIZE>(_state) {}
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Namespace         : ThisTask                                                                                       *
 *                                                                                                                    *
 * Description       : provide set of functions for Current Task                                                      *
 *                                                                                                                    *
 **********************************************************************************************************************/

namespace ThisTask
{
	static inline void     pass      ( void )            {        tsk_pass      ();                        }
	static inline void     yield     ( void )            {        tsk_yield     ();                        }
	static inline void     flip      ( FUN_t    _state ) {        ((baseTask *) Current)->_start = _state;
	                                                              tsk_flip      ((fun_t *) baseTask::run); }
	static inline void     stop      ( void )            {        tsk_stop      ();                        }
	static inline unsigned sleepUntil( uint32_t _time )  { return tsk_sleepUntil(_time);                   }
	static inline unsigned sleepFor  ( uint32_t _delay ) { return tsk_sleepFor  (_delay);                  }
	static inline unsigned sleep     ( void )            { return tsk_sleep     ();                        }
	static inline unsigned delay     ( uint32_t _delay ) { return tsk_delay     (_delay);                  }
	static inline void     suspend   ( void )            {        tsk_suspend   (Current);                 }
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_TSK_H
