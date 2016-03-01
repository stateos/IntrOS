/******************************************************************************

    @file    IntrOS: os_msg.h
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
 * Name              : message queue                                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_MSG                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a message queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : name of a pointer to message queue object                                                      *
 *   limit           : size of a queue (max number of stored messages)                                                *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_MSG( msg, limit )                                \
                       unsigned msg##__buf[limit];                       \
                       msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                       msg_id msg = & msg##__msg

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_MSG                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static message queue object                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : name of a pointer to message queue object                                                      *
 *   limit           : size of a queue (max number of stored messages)                                                *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_MSG( msg, limit )                                \
                static unsigned msg##__buf[limit];                       \
                static msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                static msg_id msg = & msg##__msg

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MSG_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a message queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored messages)                                                *
 *                                                                                                                    *
 * Return            : message queue object                                                                           *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                MSG_INIT( limit ) \
                      _MSG_INIT( limit, _MSG_DATA( limit ) )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : MSG_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a message queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored messages)                                                *
 *                                                                                                                    *
 * Return            : pointer to message queue object                                                                *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                MSG_CREATE( limit ) \
               &(msg_t)MSG_INIT( limit )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer message data from the message queue object,                                    *
 *                     wait indefinitly while the message queue object is empty                                       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : pointer to store message data                                                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     msg_wait( msg_id msg, unsigned *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer message data from the message queue object,                                    *
 *                     don't wait if the message queue object is empty                                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : pointer to store message data                                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered from the message queue object                         *
 *   E_FAILURE       : message queue object is empty                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned msg_take( msg_id msg, unsigned *data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_send                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer message data to the message queue object,                                      *
 *                     wait indefinitly while the message queue object is full                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : message data                                                                                   *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     msg_send( msg_id msg, unsigned data );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : msg_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer message data to the message queue object,                                      *
 *                     don't wait if the message queue object is full                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   msg             : pointer to message queue object                                                                *
 *   data            : message data                                                                                   *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : message data was successfully transfered to the message queue object                           *
 *   E_FAILURE       : message queue object is full                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned msg_give( msg_id msg, unsigned data );

#ifdef __cplusplus
}
#endif
