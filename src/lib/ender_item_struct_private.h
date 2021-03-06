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

#ifndef _ENDER_ITEM_STRUCT_PRIVATE_H_
#define _ENDER_ITEM_STRUCT_PRIVATE_H_

#include "ender_item_private.h"

Ender_Item * ender_item_struct_new(void);
void ender_item_struct_field_add(Ender_Item *i, Ender_Item *f);

Eina_Bool ender_item_struct_field_value_set(void *o, Ender_Item *field,
		Ender_Value *v, Eina_Error *err);
Eina_Bool ender_item_struct_field_value_get(void *o, Ender_Item *field,
		Ender_Value *v, Eina_Error *err);

void ender_item_struct_function_add(Ender_Item *i, Ender_Item *f);

#endif
