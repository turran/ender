/* ENDER - Enesim's descriptor library
 * Copyright (C) 2010 - 2012 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ENDER_INSTANCE_H_
#define ENDER_INSTANCE_H_

#define ENDER_INSTANCE_DESCRIPTOR_CTOR(f) (Ender_Instance_Descriptor_Ctor)(f)
#define ENDER_INSTANCE_DESCRIPTOR_DTOR(f) (Ender_Instance_Descriptor_Dtor)(f)
#define ENDER_INSTANCE_DESCRIPTOR_PROCESS(f) (Ender_Instance_Descriptor_Process)(f)
#define ENDER_INSTANCE_DESCRIPTOR_POPULATE(f) (Ender_Instance_Descriptor_Populate)(f)
#define ENDER_INSTANCE_DESCRIPTOR_CHILD_APPENDABLE(f) (Ender_Instance_Descriptor_Child_Appendable)(f)
#define ENDER_INSTANCE_DESCRIPTOR_CHILD_ADDED(f) (Ender_Instance_Descriptor_Child_Added)(f)
#define ENDER_INSTANCE_DESCRIPTOR_CHILD_REMOVED(f) (Ender_Instance_Descriptor_Child_Removed)(f)

typedef void * (*Ender_Instance_Descriptor_Ctor)(void);
typedef void (*Ender_Instance_Descriptor_Dtor)(void *thiz);
typedef void (*Ender_Instance_Descriptor_Populate)(Egueb_Dom_Node *n, void *thiz);
typedef Eina_Bool (*Ender_Instance_Descriptor_Process)(Egueb_Dom_Node *n, void *thiz);
typedef Eina_Bool (*Ender_Instance_Descriptor_Child_Appendable)(Egueb_Dom_Node *n,
		void *thiz, Egueb_Dom_Node *child);
typedef void (*Ender_Instance_Descriptor_Child_Added)(Egueb_Dom_Node *n, void *thiz,
		Egueb_Dom_Node *child);
typedef void (*Ender_Instance_Descriptor_Child_Removed)(Egueb_Dom_Node *n, void *thiz,
		Egueb_Dom_Node *child);

typedef struct _Ender_Instance_Descriptor {
	Ender_Instance_Descriptor_Ctor ctor;
	Ender_Instance_Descriptor_Dtor dtor;
	Ender_Instance_Descriptor_Populate populate;
	Ender_Instance_Descriptor_Process process;
	Ender_Instance_Descriptor_Child_Appendable child_appendable;
	Ender_Instance_Descriptor_Child_Added child_added;
	Ender_Instance_Descriptor_Child_Removed child_removed;
} Ender_Instance_Descriptor;

#endif
