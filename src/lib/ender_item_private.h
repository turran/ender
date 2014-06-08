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

#ifndef _ENDER_ITEM_PRIVATE_H_
#define _ENDER_ITEM_PRIVATE_H_

#include "ender_lib.h"

Enesim_Object_Descriptor * ender_item_descriptor_get(void);
#define ENDER_ITEM_DESCRIPTOR ender_item_descriptor_get()
#define ENDER_ITEM_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, Ender_Item_Class, ENDER_ITEM_DESCRIPTOR)
#define ENDER_ITEM_CLASS_GET(o) ENDER_ITEM_CLASS(ENESIM_OBJECT_INSTANCE_CLASS(o))
#define ENDER_ITEM(o) ENESIM_OBJECT_INSTANCE_CHECK(o, Ender_Item, ENDER_ITEM_DESCRIPTOR)

typedef struct _Ender_Item
{
	Enesim_Object_Instance base;
	Ender_Lib *lib;
	Ender_Item *parent;
	Ender_Item_Type type;
	char *name;
	int ref;
} Ender_Item;

typedef struct _Ender_Item_Class
{
	Enesim_Object_Class base;
} Ender_Item_Class;

void ender_item_name_set(Ender_Item *thiz, const char *name);
void ender_item_parent_set(Ender_Item *thiz, Ender_Item *parent);
void * ender_item_sym_get(Ender_Item *thiz, const char *name);

#endif
