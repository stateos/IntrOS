/******************************************************************************

    @file    IntrOS: os_box.h
    @author  Rajmund Szymanski
    @date    01.03.2016
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

#pragma once

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mailbox queue                                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

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

#define             OS_BOX( box, limit, size )                                \
                       char box##__buf[limit*size];                            \
                       box_t box##__box = _BOX_INIT( limit, size, box##__buf ); \
                       box_id box = & box##__box

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

#define         static_BOX( box, limit, size )                                \
                static char box##__buf[limit*size];                            \
                static box_t box##__box = _BOX_INIT( limit, size, box##__buf ); \
                static box_id box = & box##__box

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
