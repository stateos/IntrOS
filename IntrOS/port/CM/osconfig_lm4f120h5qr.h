/******************************************************************************

    @file    IntrOS: osconfig.h
    @author  Rajmund Szymanski
    @date    03.02.2016
    @brief   IntrOS config file for LM4F uC.

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

#include <lm4f120h5qr.h>

// ----------------------------
// cpu frequency in Hz
// default value: none
#define CPU_FREQUENCY  80000000

// ----------------------------
// os frequency in Hz
// dafault value: 1000
#define  OS_FREQUENCY      1000

// ----------------------------
// default task stack size in bytes
// default value: 256
#define  OS_STACK_SIZE      256
