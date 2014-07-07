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

#ifndef _ENDER_ITEM_H_
#define _ENDER_ITEM_H_

/*
 * @defgroup Ender_Item_Group Item
 * @{
 */

typedef enum _Ender_Item_Type {
	ENDER_ITEM_TYPE_INVALID,
	ENDER_ITEM_TYPE_BASIC,
	ENDER_ITEM_TYPE_FUNCTION,
	ENDER_ITEM_TYPE_ATTR,
	ENDER_ITEM_TYPE_ARG,
	ENDER_ITEM_TYPE_OBJECT,
	ENDER_ITEM_TYPE_STRUCT,
	ENDER_ITEM_TYPE_CONSTANT,
	ENDER_ITEM_TYPE_ENUM,
	ENDER_ITEM_TYPE_DEF,
} Ender_Item_Type;

EAPI Ender_Item * ender_item_ref(Ender_Item *thiz);
EAPI void ender_item_unref(Ender_Item *thiz);
EAPI const char * ender_item_name_get(Ender_Item *thiz);
EAPI Ender_Item_Type ender_item_type_get(Ender_Item *thiz);
EAPI Ender_Item * ender_item_parent_get(Ender_Item *thiz);
EAPI const char * ender_item_type_name_get(Ender_Item_Type type);
EAPI const Ender_Lib * ender_item_lib_get(Ender_Item *thiz);
EAPI Eina_Bool ender_item_is_exception(Ender_Item *i);

/**
 * @}
 */

#endif
