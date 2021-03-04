/******************************************************************************

    @file    IntrOS: osflag.h
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

#ifndef __INTROS_FLG_H
#define __INTROS_FLG_H

#include "oskernel.h"

/* -------------------------------------------------------------------------- */

#define flgAny        ( false )
#define flgAll        ( true  )

/******************************************************************************
 *
 * Name              : flag
 *
 ******************************************************************************/

typedef struct __flg flg_t, * const flg_id;

struct __flg
{
	unsigned flags; // pending flags
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _FLG_INIT
 *
 * Description       : create and initialize a flag object
 *
 * Parameters
 *   init            : initial value of flag
 *
 * Return            : flag object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _FLG_INIT( _init ) { _init }

/******************************************************************************
 *
 * Name              : _VA_FLG
 *
 * Description       : calculate initial value of flag from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_FLG( _init ) ( _init + 0 )

/******************************************************************************
 *
 * Name              : OS_FLG
 *
 * Description       : define and initialize a flag object
 *
 * Parameters
 *   flg             : name of a pointer to flag object
 *   init            : (optional) initial value of flag; default: 0
 *
 ******************************************************************************/

#define             OS_FLG( flg, ... )                                      \
                       flg_t flg##__flg = _FLG_INIT( _VA_FLG(__VA_ARGS__) ); \
                       flg_id flg = & flg##__flg

/******************************************************************************
 *
 * Name              : static_FLG
 *
 * Description       : define and initialize a static flag object
 *
 * Parameters
 *   flg             : name of a pointer to flag object
 *   init            : (optional) initial value of flag; default: 0
 *
 ******************************************************************************/

#define         static_FLG( flg, ... )                                      \
                static flg_t flg##__flg = _FLG_INIT( _VA_FLG(__VA_ARGS__) ); \
                static flg_id flg = & flg##__flg

/******************************************************************************
 *
 * Name              : FLG_INIT
 *
 * Description       : create and initialize a flag object
 *
 * Parameters
 *   init            : (optional) initial value of flag; default: 0
 *
 * Return            : flag object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                FLG_INIT( ... ) \
                      _FLG_INIT( _VA_FLG(__VA_ARGS__) )
#endif

/******************************************************************************
 *
 * Name              : FLG_CREATE
 * Alias             : FLG_NEW
 *
 * Description       : create and initialize a flag object
 *
 * Parameters
 *   init            : (optional) initial value of flag; default: 0
 *
 * Return            : pointer to flag object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                FLG_CREATE( ... ) \
           (flg_t[]) { FLG_INIT  ( _VA_FLG(__VA_ARGS__) ) }
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
 *   init            : initial value of flag
 *
 * Return            : none
 *
 ******************************************************************************/

void flg_init( flg_t *flg, unsigned init );

/******************************************************************************
 *
 * Name              : flg_take
 * Alias             : flg_tryWait
 *
 * Description       : check if required flags have been set in the flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to wait
 *   all             : waiting mode
 *                     flgAny: wait for any flags to be set
 *                     flgAll: wait for all flags to be set
 *
 * Return            : remaining flags
 *
 ******************************************************************************/

unsigned flg_take( flg_t *flg, unsigned flags, bool all );

__STATIC_INLINE
unsigned flg_tryWait( flg_t *flg, unsigned flags, bool all ) { return flg_take(flg, flags, all); }

/******************************************************************************
 *
 * Name              : flg_wait
 *
 * Description       : wait indefinitely on flag object until required flags have been set
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to wait
 *   all             : waiting mode
 *                     flgAny: wait for any flags to be set
 *                     flgAll: wait for all flags to be set
 *
 * Return            : none
 *
 ******************************************************************************/

void flg_wait( flg_t *flg, unsigned flags, bool all );

/******************************************************************************
 *
 * Name              : flg_give
 * Alias             : flg_set
 *
 * Description       : set given flags in flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to set
 *
 * Return            : flags after setting
 *
 ******************************************************************************/

unsigned flg_give( flg_t *flg, unsigned flags );

__STATIC_INLINE
unsigned flg_set( flg_t *flg, unsigned flags ) { return flg_give(flg, flags); }

/******************************************************************************
 *
 * Name              : flg_clear
 *
 * Description       : clear given flags in flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to clear
 *
 * Return            : flags before clearing
 *
 ******************************************************************************/

unsigned flg_clear( flg_t *flg, unsigned flags );

/******************************************************************************
 *
 * Name              : flg_get
 *
 * Description       : get current flags from flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *
 * Return            : current flags in flag object
 *
 ******************************************************************************/

unsigned flg_get( flg_t *flg );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
namespace intros {

/******************************************************************************
 *
 * Class             : Flag
 *
 * Description       : create and initialize a flag object
 *
 * Constructor parameters
 *   init            : initial value of flag
 *
 ******************************************************************************/

struct Flag : public __flg
{
	constexpr
	Flag( const unsigned _init = 0 ): __flg _FLG_INIT(_init) {}

	Flag( Flag&& ) = default;
	Flag( const Flag& ) = delete;
	Flag& operator=( Flag&& ) = delete;
	Flag& operator=( const Flag& ) = delete;

	unsigned take   ( unsigned _flags, bool _all = true ) { return flg_take   (this, _flags, _all); }
	unsigned tryWait( unsigned _flags, bool _all = true ) { return flg_tryWait(this, _flags, _all); }
	void     wait   ( unsigned _flags, bool _all = true ) { return flg_wait   (this, _flags, _all); }
	unsigned give   ( unsigned _flags )                   { return flg_give   (this, _flags); }
	unsigned set    ( unsigned _flags )                   { return flg_set    (this, _flags); }
	unsigned clear  ( unsigned _flags )                   { return flg_clear  (this, _flags); }
	unsigned get    ( void )                              { return flg_get    (this); }
};

}     //  namespace
#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_FLG_H
