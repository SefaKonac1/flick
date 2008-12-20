/*
 * Copyright (c) 1995, 1996, 1997, 1999 The University of Utah and
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

#include <assert.h>
#include "private.hh"

void pg_state::p_def(aoi_type aoit)
{
	assert(aoit);
	switch (aoit->kind) {
	case AOI_CONST:
		/* Consts should be mapped to #define's */
		p_const_def(&aoit->aoi_type_u_u.const_def);
		break;
	case AOI_NAMESPACE:
		/* Namespaces are not used for anything but
		 * building scoped names
		 */
		p_namespace_def();
		break;
	case AOI_INTERFACE:
		p_interface_def(&aoit->aoi_type_u_u.interface_def);
		break;
	default:
		p_typedef_def(aoit);
	}
}

/* End of file. */
