/*
 * Copyright (c) 1999 The University of Utah and
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

#include <mom/c/libcast.h>
#include <mom/libaoi.h>

cast_expr aoi_const_to_cast_expr(aoi_const c)
{
	cast_expr retval = 0;
	
	switch(c->kind) {
	case AOI_CONST_INT:
		retval = cast_new_expr_lit_int(c->aoi_const_u_u.const_int, 0);
		break;
	case AOI_CONST_CHAR:
		retval = cast_new_expr_lit_char(c->aoi_const_u_u.const_char,
						0);
		break;
	case AOI_CONST_FLOAT:
		retval = cast_new_expr_lit_float(c->aoi_const_u_u.const_float);
		break;
	default:
		printf("Can't transform aoi_const kind %d to a cast_expr",
		       c->kind);
		break;
	}
	return( retval );
}
