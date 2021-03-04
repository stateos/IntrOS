/******************************************************************************

    @file    IntrOS: osrwlock.h
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

#ifndef __INTROS_RWL_H
#define __INTROS_RWL_H

#include "oskernel.h"

/* -------------------------------------------------------------------------- */

#define RDR_LIMIT    ( 0U-1 )

/******************************************************************************
 *
 * Name              : read/write lock (one write, many read)
 *
 ******************************************************************************/

typedef struct __rwl rwl_t, * const rwl_id;

struct __rwl
{
	bool     write; // writer is active
	unsigned count; // number of active readers
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _RWL_INIT
 *
 * Description       : create and initialize a read/write lock object
 *
 * Parameters        : none
 *
 * Return            : read/write lock object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _RWL_INIT() { false, 0 }

/******************************************************************************
 *
 * Name              : OS_RWL
 *
 * Description       : define and initialize a read/write lock object
 *
 * Parameters
 *   rwl             : name of a pointer to read/write lock object
 *
 ******************************************************************************/

#define             OS_RWL( rwl )                     \
                       rwl_t rwl##__rwl = _RWL_INIT(); \
                       rwl_id rwl = & rwl##__rwl

/******************************************************************************
 *
 * Name              : static_RWL
 *
 * Description       : define and initialize a static read/write lock object
 *
 * Parameters
 *   rwl             : name of a pointer to read/write lock object
 *
 ******************************************************************************/

#define         static_RWL( rwl )                     \
                static rwl_t rwl##__rwl = _RWL_INIT(); \
                static rwl_id rwl = & rwl##__rwl

/******************************************************************************
 *
 * Name              : RWL_INIT
 *
 * Description       : create and initialize a read/write lock object
 *
 * Parameters        : none
 *
 * Return            : read/write lock object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                RWL_INIT() \
                      _RWL_INIT()
#endif

/******************************************************************************
 *
 * Name              : RWL_CREATE
 * Alias             : RWL_NEW
 *
 * Description       : create and initialize a read/write lock object
 *
 * Parameters        : none
 *
 * Return            : pointer to read/write lock object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                RWL_CREATE() \
           (rwl_t[]) { RWL_INIT  () }
#define                RWL_NEW \
                       RWL_CREATE
#endif

/******************************************************************************
 *
 * Name              : rwl_init
 *
 * Description       : initialize a read/write lock object
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void rwl_init( rwl_t *rwl );

/******************************************************************************
 *
 * Name              : rwl_takeRead
 * Alias             : rwl_tryLockRead
 *
 * Description       : try to lock the reader,
 *                     don't wait if the reader can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return
 *   SUCCESS         : reader was successfully locked
 *   FAILURE         : reader can't be locked immediately
 *
 ******************************************************************************/

unsigned rwl_takeRead( rwl_t *rwl );

__STATIC_INLINE
unsigned rwl_tryLockRead( rwl_t *rwl ) { return rwl_takeRead(rwl); }

/******************************************************************************
 *
 * Name              : rwl_waitRead
 * Alias             : rwl_lockRead
 *
 * Description       : try to lock the reader,
 *                     wait indefinitely if the reader can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 ******************************************************************************/

void rwl_waitRead( rwl_t *rwl );

__STATIC_INLINE
void rwl_lockRead( rwl_t *rwl ) { rwl_waitRead(rwl); }

/******************************************************************************
 *
 * Name              : rwl_giveRead
 * Alias             : rwl_unlockRead
 *
 * Description       : unlock the reader
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 ******************************************************************************/

void rwl_giveRead( rwl_t *rwl );

__STATIC_INLINE
void rwl_unlockRead( rwl_t *rwl ) { rwl_giveRead(rwl); }

/******************************************************************************
 *
 * Name              : rwl_takeWrite
 * Alias             : rwl_tryLockWrite
 *
 * Description       : try to lock the writer,
 *                     don't wait if the writer can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return
 *   SUCCESS         : writer was successfully locked
 *   FAILURE         : writer can't be locked immediately
 *
 ******************************************************************************/

unsigned rwl_takeWrite( rwl_t *rwl );

__STATIC_INLINE
unsigned rwl_tryLockWrite( rwl_t *rwl ) { return rwl_takeWrite(rwl); }

/******************************************************************************
 *
 * Name              : rwl_waitWrite
 * Alias             : rwl_lockWrite
 *
 * Description       : try to lock the writer,
 *                     wait indefinitely if the writer can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 ******************************************************************************/

void rwl_waitWrite( rwl_t *rwl );

__STATIC_INLINE
void rwl_lockWrite( rwl_t *rwl ) { rwl_waitWrite(rwl); }

/******************************************************************************
 *
 * Name              : rwl_giveWrite
 * Alias             : rwl_unlockWrite
 *
 * Description       : unlock the writer
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void rwl_giveWrite( rwl_t *rwl );

__STATIC_INLINE
void rwl_unlockWrite( rwl_t *rwl ) { rwl_giveWrite(rwl); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : RWLock
 *
 * Description       : create and initialize a read/write lock object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct RWLock : public __rwl
{
	constexpr
	RWLock( void ): __rwl _RWL_INIT() {}

	RWLock( RWLock&& ) = default;
	RWLock( const RWLock& ) = delete;
	RWLock& operator=( RWLock&& ) = delete;
	RWLock& operator=( const RWLock& ) = delete;

	~RWLock( void ) { assert(__rwl::write == false && __rwl::count == 0); }

	unsigned takeRead    ( void ) { return rwl_takeRead    (this); }
	unsigned tryLockRead ( void ) { return rwl_tryLockRead (this); }
	void     waitRead    ( void ) {        rwl_waitRead    (this); }
	void     lockRead    ( void ) {        rwl_lockRead    (this); }
	void     giveRead    ( void ) {        rwl_giveRead    (this); }
	void     unlockRead  ( void ) {        rwl_unlockRead  (this); }
	unsigned takeWrite   ( void ) { return rwl_takeWrite   (this); }
	unsigned tryLockWrite( void ) { return rwl_tryLockWrite(this); }
	void     waitWrite   ( void ) {        rwl_waitWrite   (this); }
	void     lockWrite   ( void ) {        rwl_lockWrite   (this); }
	void     giveWrite   ( void ) {        rwl_giveWrite   (this); }
	void     unlockWrite ( void ) {        rwl_unlockWrite (this); }
};

/******************************************************************************
 *
 * Class             : ReadLock
 *
 * Description       : create and initialize a read guard object
 *
 * Constructor parameters
 *   rwl             : RWLock object
 *
 ******************************************************************************/

struct ReadLock
{
	 ReadLock( RWLock &_rwl ): lck_(_rwl) { lck_.lockRead(); }
	~ReadLock( void )                     { lck_.unlockRead(); }

	ReadLock( ReadLock&& ) = default;
	ReadLock( const ReadLock& ) = delete;
	ReadLock& operator=( ReadLock&& ) = delete;
	ReadLock& operator=( const ReadLock& ) = delete;

	private:
	RWLock &lck_;
};

/******************************************************************************
 *
 * Class             : WriteLock
 *
 * Description       : create and initialize a write guard object
 *
 * Constructor parameters
 *   rwl             : RWLock object
 *
 ******************************************************************************/

struct WriteLock
{
	 WriteLock( RWLock &_rwl ): lck_(_rwl) { lck_.lockWrite(); }
	~WriteLock( void )                     { lck_.unlockWrite(); }

	WriteLock( WriteLock&& ) = default;
	WriteLock( const WriteLock& ) = delete;
	WriteLock& operator=( WriteLock&& ) = delete;
	WriteLock& operator=( const WriteLock& ) = delete;

	private:
	RWLock &lck_;
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_RWL_H
