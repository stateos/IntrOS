/******************************************************************************

    @file    IntrOS: osflag.h
    @author  Rajmund Szymanski
    @date    13.05.2018
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

#ifndef __INTROS_FLG_H
#define __INTROS_FLG_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : flag
 *
 ******************************************************************************/

typedef struct __flg flg_t, * const flg_id;

struct __flg
{
	unsigned flags; // flag's current value
};

/* -------------------------------------------------------------------------- */

#define flgAny        ( false )
#define flgAll        ( true  )

/******************************************************************************
 *
 * Name              : _FLG_INIT
 *
 * Description       : create and initialize a flag object
 *
 * Parameters        : none
 *
 * Return            : flag object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _FLG_INIT() { 0 }

/******************************************************************************
 *
 * Name              : OS_FLG
 *
 * Description       : define and initialize a flag object
 *
 * Parameters
 *   flg             : name of a pointer to flag object
 *
 ******************************************************************************/

#define             OS_FLG( flg )                     \
                       flg_t flg##__flg = _FLG_INIT(); \
                       flg_id flg = & flg##__flg

/******************************************************************************
 *
 * Name              : static_FLG
 *
 * Description       : define and initialize a static flag object
 *
 * Parameters
 *   flg             : name of a pointer to flag object
 *
 ******************************************************************************/

#define         static_FLG( flg )                     \
                static flg_t flg##__flg = _FLG_INIT(); \
                static flg_id flg = & flg##__flg

/******************************************************************************
 *
 * Name              : FLG_INIT
 *
 * Description       : create and initialize a flag object
 *
 * Parameters        : none
 *
 * Return            : flag object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                FLG_INIT() \
                      _FLG_INIT()
#endif

/******************************************************************************
 *
 * Name              : FLG_CREATE
 * Alias             : FLG_NEW
 *
 * Description       : create and initialize a flag object
 *
 * Parameters        : none
 *
 * Return            : pointer to flag object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                FLG_CREATE() \
             & (flg_t) FLG_INIT  ()
#define                FLG_NEW \
                       FLG_CREATE
#endif

/******************************************************************************
 *
 * Name              : flg_init
 *
 * Description       : initialize a flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *
 * Return            : none
 *
 ******************************************************************************/

void flg_init( flg_t *flg );

/******************************************************************************
 *
 * Name              : flg_wait
 *
 * Description       : wait indefinitely on flag object until requested flags have been set
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to wait
 *   all             : waiting mode
 *                     flgAny:    wait for any flags to be set
 *                     flgAll:    wait for all flags to be set
 *
 * Return            : none
 *
 ******************************************************************************/

void flg_wait( flg_t *flg, unsigned flags, bool all );

/******************************************************************************
 *
 * Name              : flg_take
 *
 * Description       : don't wait on flag object until requested flags have been set
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to wait
 *   all             : waiting mode
 *                     flgAny:    wait for any flags to be set
 *                     flgAll:    wait for all flags to be set
 *
 * Return
 *   E_SUCCESS       : requested flags have been set
 *   'another'       : requested flags have not been set
 *
 ******************************************************************************/

unsigned flg_take( flg_t *flg, unsigned flags, bool all );

/******************************************************************************
 *
 * Name              : flg_give
 *
 * Description       : set given flags in flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to set
 *
 * Return            : none
 *
 ******************************************************************************/

void flg_give( flg_t *flg, unsigned flags );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Flag
 *
 * Description       : create and initialize a flag object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct Flag : public __flg
{
	explicit
	Flag( void ): __flg _FLG_INIT() {}

	void     wait( unsigned _flags, bool _all = true ) {        flg_wait(this, _flags, _all); }
	unsigned take( unsigned _flags, bool _all = true ) { return flg_take(this, _flags, _all); }
	void     give( unsigned _flags )                   {        flg_give(this, _flags);       }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROS_FLG_H
