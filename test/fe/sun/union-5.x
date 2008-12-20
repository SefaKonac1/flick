/*
 * Copyright (c) 1996, 1999 The University of Utah and
 * the Computer Systems Laboratory at the University of Utah (CSL).
 *
 * This file is part of Flick, the Flexible IDL Compiler Kit.
 *
 * Flick is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Flick is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Flick; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place #330, Boston, MA 02111, USA.
 */

/* This file contains test input for `flick-fe-sun'. */
/* Purpose: To test the presentation of an operation that accepts and returns a
   discriminated union with non-atomic members. */

struct bar
{
    int slot_1;
    int slot_2;
    int slot_3;
};

union foo
switch (int discriminator)
{
    case 1: int one<100>;
    case 2: string two<>;
    case 3: bar three;
};

program PROG {
    version VERS {
	foo op(foo) = 1;
    } = 2;
} = 3;

/* End of file. */
