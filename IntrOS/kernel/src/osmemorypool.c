/******************************************************************************

    @file    IntrOS: osmemorypool.c
    @author  Rajmund Szymanski
    @date    25.05.2020
    @brief   This file provides set of functions for IntrOS.

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

#include "inc/osmemorypool.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void mem_bind( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	que_t  * ptr;
	unsigned cnt;

	assert(mem);
	assert(mem->limit);
	assert(mem->size);
	assert(mem->data);

	sys_lock();
	{
		ptr = mem->data;
		cnt = mem->limit;

		mem->lst.head.next = 0;
		while (cnt--) { mem_give(mem, ++ptr); ptr += mem->size; }
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mem_init( mem_t *mem, size_t size, que_t *data, size_t bufsize )
/* -------------------------------------------------------------------------- */
{
	assert(mem);
	assert(size);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(mem, 0, sizeof(mem_t));

		mem->limit = bufsize / (1 + MEM_SIZE(size)) / sizeof(que_t);
		mem->size  = MEM_SIZE(size);
		mem->data  = data;

		mem_bind(mem);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
