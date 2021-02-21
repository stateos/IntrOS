/******************************************************************************

    @file    IntrOS: oslibc.c
    @author  Rajmund Szymanski
    @date    21.02.2021
    @brief   This file provides set of variables and functions for IntrOS.

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

#include <errno.h>
#include <sys/stat.h>
#include "oskernel.h"

/* -------------------------------------------------------------------------- */

caddr_t _sbrk_r( struct _reent *reent, size_t size )
{
	extern char __heap_start[];
	extern char __heap_end[];
	static char * heap = __heap_start;
	       char * base;
	      (void)  reent;

	if (heap + size <= __heap_end)
	{
		base  = heap;
		heap += size;
	}
	else
	{
		errno = ENOMEM;
		base  = (caddr_t) -1;
	}

	return base;
}

/* -------------------------------------------------------------------------- */

#if !defined(USE_SEMIHOST) && !defined(USE_NOHOST)

/* -------------------------------------------------------------------------- */

static
int __enosys()
{
	errno = ENOSYS; // procedure not implemented
	return -1;
}

/* -------------------------------------------------------------------------- */

int   _open_r( struct _reent *, const char *, int, int ) __attribute__((weak, alias("__enosys")));
int  _close_r( struct _reent *, int )                    __attribute__((weak, alias("__enosys")));
int  _lseek_r( struct _reent *, int, int, int )          __attribute__((weak, alias("__enosys")));
int   _read_r( struct _reent *, int, char *, size_t )    __attribute__((weak, alias("__enosys")));
int  _write_r( struct _reent *, int, char *, size_t )    __attribute__((weak, alias("__enosys")));
int _isatty_r( struct _reent *, int )                    __attribute__((weak, alias("__enosys")));
int  _fstat_r( struct _reent *, int, struct stat * )     __attribute__((weak, alias("__enosys")));
int _getpid_r( struct _reent * )                         __attribute__((weak, alias("__enosys")));
int   _kill_r( struct _reent *, int, int )               __attribute__((weak, alias("__enosys")));

/* -------------------------------------------------------------------------- */

#endif // !USE_SEMIHOST && !USE_NOHOST

/* -------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>

void __assert_func(const char* const file, const int line, const char* const func, const char* const expr)
{
	printf("\nassert error at %s:%d:%s:%s\n", file, line, expr, func);
	abort();
}

/* -------------------------------------------------------------------------- */
