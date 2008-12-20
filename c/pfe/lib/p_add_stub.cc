/*
 * Copyright (c) 1995, 1996 The University of Utah and
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

#include <string.h>

#include "private.hh"

/* Allocate a new stub entry in the specified pres_c,
   and return its index number.  */
int pg_state::p_add_stub(pres_c_1 *pres)
{
	int i;

	/* XXX horribly inefficient,
	   but needed until we get a max field... */
	i = pres->stubs.stubs_len++;
	pres->stubs.stubs_val = (pres_c_stub *)mustrealloc(pres->stubs.stubs_val,
		pres->stubs.stubs_len * sizeof(pres_c_stub));

	memset(&pres->stubs.stubs_val[i], 0, sizeof(pres->stubs.stubs_val[i]));

	return i;
}
