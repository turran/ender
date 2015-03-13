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

/**
 * @defgroup Ender_Item_Group Item
 * @brief An item represents any identifier a @ref Ender_Lib might have
 * @{
 */

/**
 * The type of an item
 * @see ender_item_type_get
 */
typedef enum _Ender_Item_Type {
	/** Impossible case */
	ENDER_ITEM_TYPE_INVALID,
	/** The item is a @ref Ender_Basic_Group */
	ENDER_ITEM_TYPE_BASIC,
	/** The item is a @ref Ender_Function_Group */
	ENDER_ITEM_TYPE_FUNCTION,
	/** The item is a @ref Ender_Attr_Group */
	ENDER_ITEM_TYPE_ATTR,
	/** The item is a @ref Ender_Arg_Group */
	ENDER_ITEM_TYPE_ARG,
	/** The item is a @ref Ender_Object_Group */
	ENDER_ITEM_TYPE_OBJECT,
	/** The item is a @ref Ender_Struct_Group */
	ENDER_ITEM_TYPE_STRUCT,
	/** The item is a @ref Ender_Constant_Group */
	ENDER_ITEM_TYPE_CONSTANT,
	/** The item is a @ref Ender_Enum_Group */
	ENDER_ITEM_TYPE_ENUM,
	/** The item is a @ref Ender_Def_Group */
	ENDER_ITEM_TYPE_DEF,
} Ender_Item_Type;

/**
 * The type of transfer
 */
typedef enum _Ender_Item_Transfer
{
	ENDER_ITEM_TRANSFER_FULL,
	ENDER_ITEM_TRANSFER_NONE,
	ENDER_ITEM_TRANSFER_CONTAINER,
	ENDER_ITEM_TRANSFER_CONTENT,
} Ender_Item_Transfer;

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
