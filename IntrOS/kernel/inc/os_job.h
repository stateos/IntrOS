/******************************************************************************

    @file    IntrOS: os_job.h
    @author  Rajmund Szymanski
    @date    14.11.2017
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

#ifndef __INTROS_JOB_H
#define __INTROS_JOB_H

#include "oskernel.h"
#include "os_box.h"

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job queue                                                                                      *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __job job_t, * const job_id;

struct __job
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	fun_t ** data;  // job queue data
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _JOB_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   data            : job queue data buffer                                                                          *
 *                                                                                                                    *
 * Return            : job queue object                                                                               *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _JOB_INIT( _limit, _data ) { 0, _limit, 0, 0, _data }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _JOB_DATA                                                                                      *
 *                                                                                                                    *
 * Description       : create a job queue data buffer                                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *                                                                                                                    *
 * Return            : job queue data buffer                                                                          *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _JOB_DATA( _limit ) (fun_t *[_limit]){ 0 }
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_JOB                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : name of a pointer to job queue object                                                          *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_JOB( job, limit )                                 \
                       fun_t *job##__buf[limit];                          \
                       job_t  job##__job = _JOB_INIT( limit, job##__buf ); \
                       job_id job = & job##__job

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_JOB                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static job queue object                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : name of a pointer to job queue object                                                          *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_JOB( job, limit )                                \
                static fun_t*job##__buf[limit];                          \
                static job_t job##__job = _JOB_INIT( limit, job##__buf ); \
                static job_id job = & job##__job

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : JOB_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *                                                                                                                    *
 * Return            : job queue object                                                                               *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                JOB_INIT( limit ) \
                      _JOB_INIT( limit, _JOB_DATA( limit ) )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : JOB_CREATE                                                                                     *
 * Alias             : JOB_NEW                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *                                                                                                                    *
 * Return            : pointer to job queue object                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                JOB_CREATE( limit ) \
             & (job_t) JOB_INIT  ( limit )
#define                JOB_NEW \
                       JOB_CREATE
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a job queue object                                                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   data            : job queue data buffer                                                                          *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void job_init( job_t *job, unsigned limit, fun_t **data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,              *
 *                     wait indefinitly while the job queue object is empty                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void job_wait( job_t *job );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,              *
 *                     don't wait if the job queue object is empty                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered from the job queue object                                 *
 *   E_FAILURE       : job queue object is empty                                                                      *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned job_take( job_t *job );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_send                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer job data to the job queue object,                                              *
 *                     wait indefinitly while the job queue object is full                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   fun             : pointer to job procedure                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void job_send( job_t *job, fun_t *fun );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer job data to the job queue object,                                              *
 *                     don't wait if the job queue object is full                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   fun             : pointer to job procedure                                                                       *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered to the job queue object                                   *
 *   E_FAILURE       : job queue object is full                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned job_give( job_t *job, fun_t *fun );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : baseJobQueue                                                                                   *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   data            : job queue data buffer                                                                          *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#if OS_FUNCTIONAL

struct baseJobQueue : public __box
{
	explicit
	baseJobQueue( const unsigned _limit, FUN_t * const _data ): __box _BOX_INIT( _limit, sizeof(FUN_t), reinterpret_cast<char *>(_data) ) {}

	void     wait( void )       { FUN_t _fun;                  box_wait(this, &_fun);                         _fun();               }
	unsigned take( void )       { FUN_t _fun; unsigned event = box_take(this, &_fun); if (event == E_SUCCESS) _fun(); return event; }
	void     send( FUN_t _fun ) {                              box_send(this, &_fun);                                               }
	unsigned give( FUN_t _fun ) {             unsigned event = box_give(this, &_fun);                                 return event; }
};

#else

struct baseJobQueue : public __job
{
	explicit
	baseJobQueue( const unsigned _limit, FUN_t * const _data ): __job _JOB_INIT( _limit, _data ) {}

	void     wait( void )       {        job_wait(this);       }
	unsigned take( void )       { return job_take(this);       }
	void     send( FUN_t _fun ) {        job_send(this, _fun); }
	unsigned give( FUN_t _fun ) { return job_give(this, _fun); }
};

#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : JobQueue                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _limit>
struct JobQueueT : public baseJobQueue
{
	explicit
	JobQueueT( void ): baseJobQueue(_limit, data_) {}

	private:
	FUN_t data_[_limit];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_JOB_H
