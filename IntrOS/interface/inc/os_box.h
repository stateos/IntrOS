/******************************************************************************

    @file    IntrOS: os_box.h
    @author  Rajmund Szymanski
    @date    05.01.2017
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

#ifndef __INTROS_BOX_H
#define __INTROS_BOX_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mailbox queue                                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __box box_t, *box_id;

struct __box
{
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	char    *data;  // queue data
	unsigned size;  // size of a single mail (in bytes)
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _BOX_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *   data            : mailbox queue data buffer                                                                      *
 *                                                                                                                    *
 * Return            : mailbox queue object                                                                           *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _BOX_INIT( _limit, _size, _data ) { 0, _limit, 0, 0, _data, _size }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _BOX_DATA                                                                                      *
 *                                                                                                                    *
 * Description       : create a mailbox queue data buffer                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : mailbox queue data buffer                                                                      *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _BOX_DATA( _limit, _size ) (char[_limit * _size]){ 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BOX_DEF                                                                                        *
 *                                                                                                                    *
 * Description       : define and initilize complete mail box queue area                                              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : name of a mailbox queue area                                                                   *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                BOX_DEF( _box, _limit, _size )                         \
                       struct { box_t obj; char data[_limit * _size]; } _box = \
                       { _BOX_INIT( _limit, _size, _box.data ), { 0 } }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_BOX                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : name of a pointer to mailbox queue object                                                      *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_BOX( box, limit, size )            \
                       BOX_DEF( box##__box, limit, size ); \
                       box_id box = & box##__box.obj

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_BOX                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static mailbox queue object                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : name of a pointer to mailbox queue object                                                      *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_BOX( box, limit, size )            \
                static BOX_DEF( box##__box, limit, size ); \
                static box_id box = & box##__box.obj

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BOX_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : mailbox queue object                                                                           *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                BOX_INIT( limit, size ) \
                      _BOX_INIT( limit, size, _BOX_DATA( limit, size ) )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BOX_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *   size            : size of a single mail (in bytes)                                                               *
 *                                                                                                                    *
 * Return            : pointer to mailbox queue object                                                                *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                BOX_CREATE( limit, size ) \
               &(box_t)BOX_INIT( limit, size )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data from the mailbox queue object,                                    *
 *                     wait indefinitly while the mailbox queue object is empty                                       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to store mailbox data                                                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     box_wait( box_id box, void *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data from the mailbox queue object,                                    *
 *                     don't wait if the mailbox queue object is empty                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to store mailbox data                                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered from the mailbox queue object                         *
 *   E_FAILURE       : mailbox queue object is empty                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned box_take( box_id box, void *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_send                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data to the mailbox queue object,                                      *
 *                     wait indefinitly while the mailbox queue object is full                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to mailbox data                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     box_send( box_id box, void *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : box_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer mailbox data to the mailbox queue object,                                      *
 *                     don't wait if the mailbox queue object is full                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   box             : pointer to mailbox queue object                                                                *
 *   data            : pointer to mailbox data                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mailbox data was successfully transfered to the mailbox queue object                           *
 *   E_FAILURE       : mailbox queue object is full                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned box_give( box_id box, void *data );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : MailBoxQueue                                                                                   *
 *                                                                                                                    *
 * Description       : create and initilize a mailbox queue object                                                    *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   T               : class of a single mail                                                                         *
 *   limit           : size of a queue (max number of stored mails)                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<class T, unsigned _limit>
class MailBoxQueueT : public __box
{
	T _data[_limit];

public:

	explicit
	MailBoxQueueT( void ): __box _BOX_INIT(_limit, sizeof(T), reinterpret_cast<char *>(_data)) {}

	void     wait( T *_data ) {        box_wait(this, _data); }
	unsigned take( T *_data ) { return box_take(this, _data); }
	void     send( T *_data ) {        box_send(this, _data); }
	unsigned give( T *_data ) { return box_give(this, _data); }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_BOX_H
