/******************************************************************************

    @file    IntrOS: osmutex.h
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

#ifndef __INTROS_MTX_H
#define __INTROS_MTX_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : mutex
 *
 ******************************************************************************/

typedef struct __mtx mtx_t, * const mtx_id;

struct __mtx
{
	tsk_t  * owner; // mutex owner
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _MTX_INIT
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters        : none
 *
 * Return            : mutex object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MTX_INIT() { NULL }

/******************************************************************************
 *
 * Name              : OS_MTX
 *
 * Description       : define and initialize a mutex object
 *
 * Parameters
 *   mtx             : name of a pointer to mutex object
 *
 ******************************************************************************/

#define             OS_MTX( mtx )                     \
                       mtx_t mtx##__mtx = _MTX_INIT(); \
                       mtx_id mtx = & mtx##__mtx

/******************************************************************************
 *
 * Name              : static_MTX
 *
 * Description       : define and initialize a static mutex object
 *
 * Parameters
 *   mtx             : name of a pointer to mutex object
 *
 ******************************************************************************/

#define         static_MTX( mtx )                     \
                static mtx_t mtx##__mtx = _MTX_INIT(); \
                static mtx_id mtx = & mtx##__mtx

/******************************************************************************
 *
 * Name              : MTX_INIT
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters        : none
 *
 * Return            : mutex object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MTX_INIT() \
                      _MTX_INIT()
#endif

/******************************************************************************
 *
 * Name              : MTX_CREATE
 * Alias             : MTX_NEW
 *
 * Description       : create and initialize a mutex object
 *
 * Parameters        : none
 *
 * Return            : pointer to mutex object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MTX_CREATE() \
           (mtx_t[]) { MTX_INIT  () }
#define                MTX_NEW \
                       MTX_CREATE
#endif

/******************************************************************************
 *
 * Name              : mtx_init
 *
 * Description       : initialize a mutex object
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return            : none
 *
 ******************************************************************************/

void mtx_init( mtx_t *mtx );

/******************************************************************************
 *
 * Name              : mtx_take
 * Alias             : mtx_tryLock
 *
 * Description       : try to lock the mutex object,
 *                     don't wait if the mutex object can't be locked immediately
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return
 *   SUCCESS         : mutex object was successfully locked
 *   FAILURE         : mutex object can't be locked immediately
 *
 ******************************************************************************/

unsigned mtx_take( mtx_t *mtx );

__STATIC_INLINE
unsigned mtx_tryLock( mtx_t *mtx ) { return mtx_take(mtx); }

/******************************************************************************
 *
 * Name              : mtx_wait
 * Alias             : mtx_lock
 *
 * Description       : try to lock the mutex object,
 *                     wait indefinitely if the mutex object can't be locked immediately
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return            : none
 *
 ******************************************************************************/

void mtx_wait( mtx_t *mtx );

__STATIC_INLINE
void mtx_lock( mtx_t *mtx ) { mtx_wait(mtx); }

/******************************************************************************
 *
 * Name              : mtx_give
 * Alias             : mtx_unlock
 *
 * Description       : try to unlock the mutex object (only owner task can unlock mutex object),
 *                     don't wait if the mutex object can't be unlocked
 *
 * Parameters
 *   mtx             : pointer to mutex object
 *
 * Return
 *   SUCCESS         : mutex object was successfully unlocked
 *   FAILURE         : mutex object can't be unlocked
 *
 ******************************************************************************/

unsigned mtx_give( mtx_t *mtx );

__STATIC_INLINE
unsigned mtx_unlock( mtx_t *mtx ) { return mtx_give(mtx); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : Mutex
 *
 * Description       : create and initialize a mutex object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct Mutex : public __mtx
{
	constexpr
	Mutex( void ): __mtx _MTX_INIT() {}

	Mutex( Mutex&& ) = default;
	Mutex( const Mutex& ) = delete;
	Mutex& operator=( Mutex&& ) = delete;
	Mutex& operator=( const Mutex& ) = delete;

	~Mutex( void ) { assert(__mtx::owner == nullptr); }

	unsigned take   ( void ) { return mtx_take   (this); }
	unsigned tryLock( void ) { return mtx_tryLock(this); }
	void     wait   ( void ) {        mtx_wait   (this); }
	void     lock   ( void ) {        mtx_lock   (this); }
	unsigned give   ( void ) { return mtx_give   (this); }
	unsigned unlock ( void ) { return mtx_unlock (this); }
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_MTX_H
