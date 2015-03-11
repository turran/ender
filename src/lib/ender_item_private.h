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

typedef struct _Ender_Item_Descriptor Ender_Item_Descriptor;

typedef struct _Ender_Item
{
	Ender_Lib *lib;
	Ender_Item_Descriptor *desc;
	void *data;
	Ender_Item *parent;
	Ender_Item_Type type;
	char *name;
	int ref;
	int cycle_ref;
} Ender_Item;

typedef void (*Ender_Item_Descriptor_Init)(Ender_Item *thiz);
typedef void (*Ender_Item_Descriptor_Deinit)(Ender_Item *thiz);

struct _Ender_Item_Descriptor
{
	Ender_Item_Descriptor_Init init;
	Ender_Item_Descriptor_Deinit deinit;
};

Ender_Item * ender_item_new(Ender_Item_Descriptor *desc, void *data);
void * ender_item_data_get(Ender_Item *thiz);

void ender_item_name_set(Ender_Item *thiz, const char *name);
void ender_item_parent_set(Ender_Item *thiz, Ender_Item *parent);
void * ender_item_sym_get(Ender_Item *thiz, const char *name);
int ender_item_ref_count(Ender_Item *thiz);
void ender_item_cycle_ref_set(Ender_Item *thiz, int ref);

#endif
