/******************************************************************************

    @file    IntrOS: osconditionvariable.h
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

#ifndef __INTROS_CND_H
#define __INTROS_CND_H

#include "oskernel.h"
#include "osmutex.h"

/******************************************************************************
 *
 * Name              : condition variable
 *
 ******************************************************************************/

typedef struct __cnd cnd_t, * const cnd_id;

struct __cnd
{
	unsigned signal;
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _CND_INIT
 *
 * Description       : create and initialize a condition variable object
 *
 * Parameters        : none
 *
 * Return            : condition variable object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _CND_INIT() { 0 }

/******************************************************************************
 *
 * Name              : OS_CND
 *
 * Description       : define and initialize a condition variable object
 *
 * Parameters
 *   cnd             : name of a pointer to condition variable object
 *
 ******************************************************************************/

#define             OS_CND( cnd )                     \
                       cnd_t cnd##__cnd = _CND_INIT(); \
                       cnd_id cnd = & cnd##__cnd

/******************************************************************************
 *
 * Name              : static_CND
 *
 * Description       : define and initialize a static condition variable object
 *
 * Parameters
 *   cnd             : name of a pointer to condition variable object
 *
 ******************************************************************************/

#define         static_CND( cnd )                     \
                static cnd_t cnd##__cnd = _CND_INIT(); \
                static cnd_id cnd = & cnd##__cnd

/******************************************************************************
 *
 * Name              : CND_INIT
 *
 * Description       : create and initialize a condition variable object
 *
 * Parameters        : none
 *
 * Return            : condition variable object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                CND_INIT() \
                      _CND_INIT()
#endif

/******************************************************************************
 *
 * Name              : CND_CREATE
 * Alias             : CND_NEW
 *
 * Description       : create and initialize a condition variable object
 *
 * Parameters        : none
 *
 * Return            : pointer to condition variable object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                CND_CREATE() \
           (cnd_t[]) { CND_INIT  () }
#define                CND_NEW \
                       CND_CREATE
#endif

/******************************************************************************
 *
 * Name              : cnd_init
 *
 * Description       : initialize a condition variable object
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *
 * Return            : none
 *
 ******************************************************************************/

void cnd_init( cnd_t *cnd );

/******************************************************************************
 *
 * Name              : cnd_wait
 *
 * Description       : wait indefinitely on the condition variable releasing the currently owned mutex,
 *                     and finally lock the mutex again
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *   mtx             : currently owned mutex
 *
 * Return            : none
 *
 ******************************************************************************/

void cnd_wait( cnd_t *cnd, mtx_t *mtx );

/******************************************************************************
 *
 * Name              : cnd_give
 * Alias             : cnd_notifyAll
 *
 * Description       : signal all tasks that are waiting on the condition variable
 *
 * Parameters
 *   cnd             : pointer to condition variable object
 *
 * Return            : none
 *
 ******************************************************************************/

void cnd_give( cnd_t *cnd );

__STATIC_INLINE
void cnd_notifyAll( cnd_t *cnd ) { cnd_give(cnd); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : ConditionVariable
 *
 * Description       : create and initialize a condition variable object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct ConditionVariable : public __cnd
{
	constexpr
	ConditionVariable( void ): __cnd _CND_INIT() {}

	ConditionVariable( ConditionVariable&& ) = default;
	ConditionVariable( const ConditionVariable& ) = delete;
	ConditionVariable& operator=( ConditionVariable&& ) = delete;
	ConditionVariable& operator=( const ConditionVariable& ) = delete;

	void wait     ( mtx_t *_mtx ) { cnd_wait     (this,  _mtx); }
	void wait     ( mtx_t &_mtx ) { cnd_wait     (this, &_mtx); }
	void give     ( void )        { cnd_give     (this); }
	void notifyAll( void )        { cnd_notifyAll(this); }
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_CND_H
