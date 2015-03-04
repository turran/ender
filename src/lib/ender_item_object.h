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

#ifndef _ENDER_ITEM_OBJECT_H_
#define _ENDER_ITEM_OBJECT_H_

/**
 * @defgroup Ender_Object_Group Object
 * @brief An object represents opaque structs
 *
 * Basically an object must have constructors, a destructor and reference
 * counting. It can also have functions and properties, and can inherit
 * from another object.
 * @{
 */

EAPI Eina_Bool ender_item_object_string_to(Ender_Item *i, void *o, char **str,
		Ender_Item_Transfer *xfer, Eina_Error *err);
EAPI Ender_Item * ender_item_object_downcast(Ender_Item *i, void *o);
EAPI Ender_Item * ender_item_object_inherit_get(Ender_Item *i);
EAPI Eina_List * ender_item_object_functions_get(Ender_Item *i);
EAPI Eina_List * ender_item_object_ctor_get(Ender_Item *i);
EAPI Eina_List * ender_item_object_props_get(Ender_Item *i);
EAPI Eina_Bool ender_item_object_ref(Ender_Item *i, void *o);
EAPI Eina_Bool ender_item_object_unref(Ender_Item *i, void *o);

#endif
