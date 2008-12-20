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

/* Include the Flick-generated header for the server. */
#include "phoneS.h"

/*
 * This is our phonebook implementation class.  We need to subclass from the
 * POA class generated by Flick and then implement the virtual functions.
 */
class phone_i : public POA_data::phonebook
{
public:
	phone_i();
	~phone_i();
	
	/* The functions that implement the `phone.idl' interface. */
	virtual void add(const data::entry &e, CORBA::Environment &);
	virtual void remove(const char *n, CORBA::Environment &);
	virtual data::phone find(const char *n, CORBA::Environment &);
	
private:
	/* We implement the phonebook with a simple linked list. */
	struct impl_node_t {
		data::entry		entry;
		struct impl_node_t *	next;
	};
	typedef struct impl_node_t *impl_node_ptr;
	
	/* The head of the linked list. */
	impl_node_ptr head;
	
	/*
	 * `find_node' locates and returns an `impl_node_ptr' to an existing
	 * node that contains the given name, or a null pointer if no node
	 * contains the name.
	 */
	impl_node_ptr find_node(const char *name);
};

/* End of file. */
