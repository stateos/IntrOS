/******************************************************************************

    @file    IntrOS: osjobqueue.h
    @author  Rajmund Szymanski
    @date    16.11.2018
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

#ifndef __INTROS_JOB_H
#define __INTROS_JOB_H

#include "oskernel.h"
#include "osmailboxqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : job queue
 *
 ******************************************************************************/

typedef struct __job job_t, * const job_id;

struct __job
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	fun_t ** data;  // data buffer
};

/******************************************************************************
 *
 * Name              : _JOB_INIT
 *
 * Description       : create and initialize a job queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *   data            : job queue data buffer
 *
 * Return            : job queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _JOB_INIT( _limit, _data ) { 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _JOB_DATA
 *
 * Description       : create a job queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : job queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _JOB_DATA( _limit ) (fun_t *[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_JOB
 *
 * Description       : define and initialize a job queue object
 *
 * Parameters
 *   job             : name of a pointer to job queue object
 *   limit           : size of a queue (max number of stored job procedures)
 *
 ******************************************************************************/

#define             OS_JOB( job, limit )                                 \
                       fun_t *job##__buf[limit];                          \
                       job_t  job##__job = _JOB_INIT( limit, job##__buf ); \
                       job_id job = & job##__job

/******************************************************************************
 *
 * Name              : static_JOB
 *
 * Description       : define and initialize a static job queue object
 *
 * Parameters
 *   job             : name of a pointer to job queue object
 *   limit           : size of a queue (max number of stored job procedures)
 *
 ******************************************************************************/

#define         static_JOB( job, limit )                                 \
                static fun_t *job##__buf[limit];                          \
                static job_t  job##__job = _JOB_INIT( limit, job##__buf ); \
                static job_id job = & job##__job

/******************************************************************************
 *
 * Name              : JOB_INIT
 *
 * Description       : create and initialize a job queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : job queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                JOB_INIT( limit ) \
                      _JOB_INIT( limit, _JOB_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : JOB_CREATE
 * Alias             : JOB_NEW
 *
 * Description       : create and initialize a job queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : pointer to job queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                JOB_CREATE( limit ) \
           (job_t[]) { JOB_INIT  ( limit ) }
#define                JOB_NEW \
                       JOB_CREATE
#endif

/******************************************************************************
 *
 * Name              : job_init
 *
 * Description       : initialize a job queue object
 *
 * Parameters
 *   job             : pointer to job queue object
 *   data            : job queue data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 ******************************************************************************/

void job_init( job_t *job, fun_t **data, unsigned bufsize );

/******************************************************************************
 *
 * Name              : job_take
 * Alias             : job_tryWait
 *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,
 *                     don't wait if the job queue object is empty
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred from the job queue object
 *   E_FAILURE       : job queue object is empty
 *
 ******************************************************************************/

unsigned job_take( job_t *job );

__STATIC_INLINE
unsigned job_tryWait( job_t *job ) { return job_take(job); }

/******************************************************************************
 *
 * Name              : job_wait
 *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,
 *                     wait indefinitely while the job queue object is empty
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return            : none
 *
 ******************************************************************************/

void job_wait( job_t *job );

/******************************************************************************
 *
 * Name              : job_give
 *
 * Description       : try to transfer job data to the job queue object,
 *                     don't wait if the job queue object is full
 *
 * Parameters
 *   job             : pointer to job queue object
 *   fun             : pointer to job procedure
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred to the job queue object
 *   E_FAILURE       : job queue object is full
 *
 ******************************************************************************/

unsigned job_give( job_t *job, fun_t *fun );

/******************************************************************************
 *
 * Name              : job_send
 *
 * Description       : try to transfer job data to the job queue object,
 *                     wait indefinitely while the job queue object is full
 *
 * Parameters
 *   job             : pointer to job queue object
 *   fun             : pointer to job procedure
 *
 * Return            : none
 *
 ******************************************************************************/

void job_send( job_t *job, fun_t *fun );

/******************************************************************************
 *
 * Name              : job_push
 *
 * Description       : transfer job data to the job queue object,
 *                     remove the oldest job data if the job queue object is full
 *
 * Parameters
 *   job             : pointer to job queue object
 *   fun             : pointer to job procedure
 *
 * Return            : none
 *
 ******************************************************************************/

void job_push( job_t *job, fun_t *fun );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : staticJobQueueT<>
 *
 * Description       : create and initialize a static job queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *   data            : job queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

template<unsigned limit_>
struct staticJobQueueT : public __job
{
	staticJobQueueT( void ): __job _JOB_INIT(limit_, data_) {}

	unsigned take   ( void )        { return job_take   (this);       }
	unsigned tryWait( void )        { return job_tryWait(this);       }
	void     wait   ( void )        {        job_wait   (this);       }
	unsigned give   ( fun_t *_fun ) { return job_give   (this, _fun); }
	void     send   ( fun_t *_fun ) {        job_send   (this, _fun); }
	void     push   ( fun_t *_fun ) {        job_push   (this, _fun); }
	unsigned count  ( void )        { return job_count  (this);       }
	unsigned space  ( void )        { return job_space  (this);       }
	unsigned limit  ( void )        { return job_limit  (this);       }

	private:
	fun_t *data_[limit_];
};

/******************************************************************************
 *
 * Class             : JobQueueT<>
 *
 * Description       : create and initialize a job queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 ******************************************************************************/

#if OS_FUNCTIONAL

template<unsigned limit_>
struct JobQueueT : public __box
{
	JobQueueT( void ): __box _BOX_INIT(limit_, sizeof(FUN_t), reinterpret_cast<char *>(data_)) {}

	unsigned take   ( void )       { FUN_t _fun; unsigned event = box_take   (this, &_fun); if (event == E_SUCCESS) _fun(); return event; }
	unsigned tryWait( void )       { FUN_t _fun; unsigned event = box_tryWait(this, &_fun); if (event == E_SUCCESS) _fun(); return event; }
	void     wait   ( void )       { FUN_t _fun;                  box_wait   (this, &_fun);                         _fun();               }
	unsigned give   ( FUN_t _fun ) {             unsigned event = box_give   (this, &_fun);                                 return event; }
	void     send   ( FUN_t _fun ) {                              box_send   (this, &_fun);                                               }
	void     push   ( FUN_t _fun ) {                              box_push   (this, &_fun);                                               }
	unsigned count  ( void )       {             unsigned count = box_count  (this);                                        return count; }
	unsigned space  ( void )       {             unsigned space = box_space  (this);                                        return space; }
	unsigned limit  ( void )       {             unsigned limit = box_limit  (this);                                        return limit; }

	private:
	FUN_t data_[limit_];
};

#else

template<unsigned limit_>
struct JobQueueT : public staticJobQueueT<limit_>
{
	JobQueueT( void ): staticJobQueueT<limit_>() {}
};

#endif

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_JOB_H
