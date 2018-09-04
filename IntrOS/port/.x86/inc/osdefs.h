/******************************************************************************

    @file    IntrOS: osdefs.h
    @author  Rajmund Szymanski
    @date    16.07.2018
    @brief   IntrOS port file for X86.

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

#ifndef __INTROSDEFS_H
#define __INTROSDEFS_H

/* -------------------------------------------------------------------------- */

#ifndef __ASM
#define __ASM               __asm__
#endif
#ifndef __CONSTRUCTOR
#define __CONSTRUCTOR       __attribute__((constructor))
#endif
#ifndef __NO_RETURN
#define __NO_RETURN         __attribute__((noreturn))
#endif
#ifndef __STATIC_INLINE
#define __STATIC_INLINE       static inline
#endif
#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE  static inline \
                            __attribute__((always_inline))
#endif

/* -------------------------------------------------------------------------- */

#endif//__INTROSDEFS_H
