/******************************************************************************

    @file    IntrOS: os_sem.h
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

#ifndef __INTROS_SEM_H
#define __INTROS_SEM_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : semaphore                                                                                      *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __sem sem_t, * const sem_id;

struct __sem
{
	unsigned count; // semaphore's current value
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _SEM_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a semaphore object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   init            : initial value of semaphore counter                                                             *
 *                                                                                                                    *
 * Return            : semaphore object                                                                               *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _SEM_INIT( count ) { count }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_SEM                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a semaphore object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sem             : name of a pointer to semaphore object                                                          *
 *   init            : initial value of semaphore counter                                                             *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_SEM( sem, init )                     \
                       sem_t sem##__sem = _SEM_INIT( init ); \
                       sem_id sem = & sem##__sem

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_SEM                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static semaphore object                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sem             : name of a pointer to semaphore object                                                          *
 *   init            : initial value of semaphore counter                                                             *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_SEM( sem, init )                     \
                static sem_t sem##__sem = _SEM_INIT( init ); \
                static sem_id sem = & sem##__sem

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SEM_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a semaphore object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   init            : initial value of semaphore counter                                                             *
 *                                                                                                                    *
 * Return            : semaphore object                                                                               *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                SEM_INIT( init ) \
                      _SEM_INIT( init )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SEM_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a semaphore object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   init            : initial value of semaphore counter                                                             *
 *                                                                                                                    *
 * Return            : pointer to semaphore object                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                SEM_CREATE( init ) \
             & (sem_t) SEM_INIT( init )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sem_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to lock the semaphore object,                                                              *
 *                     wait indefinitly if the semaphore object can't be locked immediately                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sem             : pointer to semaphore object                                                                    *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void sem_wait( sem_t *sem );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sem_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to lock the semaphore object,                                                              *
 *                     don't wait if the semaphore object can't be locked immediately                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sem             : pointer to semaphore object                                                                    *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : semaphore object was successfully locked                                                       *
 *   E_FAILURE       : semaphore object can't be locked immediately                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned sem_take( sem_t *sem );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sem_send                                                                                       *
 *                                                                                                                    *
 * Description       : try to unlock the semaphore object,                                                            *
 *                     wait indefinitly if the semaphore object can't be unlocked immediately                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sem             : pointer to semaphore object                                                                    *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void sem_send( sem_t *sem );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sem_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to unlock the semaphore object,                                                            *
 *                     don't wait if the semaphore object can't be unlocked immediately                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sem             : pointer to semaphore object                                                                    *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : semaphore object was successfully unlocked                                                     *
 *   E_FAILURE       : semaphore object can't be unlocked immediately                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned sem_give( sem_t *sem );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Semaphore                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a semaphore object                                                        *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   init            : initial value of semaphore counter                                                             *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Semaphore : public __sem
{
	explicit
	Semaphore( const unsigned _init ): __sem _SEM_INIT() { count = _init; }

	void     wait( void ) {        sem_wait(this); }
	unsigned take( void ) { return sem_take(this); }
	void     send( void ) {        sem_send(this); }
	unsigned give( void ) { return sem_give(this); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_SEM_H
