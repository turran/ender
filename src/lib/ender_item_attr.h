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

#ifndef _ENDER_ITEM_ATTR_H_
#define _ENDER_ITEM_ATTR_H_

/**
 * @defgroup Ender_Attr_Group Attr
 * @brief An attr represents a @ref Ender_Object_Group attribute
 * @{
 */

/**
 * Different flags an attribute can have
 */
typedef enum _Ender_Item_Attr_Flag
{
	/**
	 * In case the attribute represents a way to convert the item into 
	 * another type
	 */
	ENDER_ITEM_ATTR_FLAG_VALUE_OF  = (1 << 0),
	/**
	 * In case the attribute returns an @ref Ender_Item_Group representing
	 * the downcasted value of the owner of the attribute
	 */
	ENDER_ITEM_ATTR_FLAG_DOWNCAST  = (1 << 1),
} Ender_Item_Attr_Flag;

EAPI Ender_Item * ender_item_attr_type_get(Ender_Item *i);
EAPI ssize_t ender_item_attr_offset_get(Ender_Item *i);
EAPI Eina_Bool ender_item_attr_value_get(Ender_Item *i, void *o, Ender_Item_Transfer *xfer,
		Ender_Value *v, Eina_Error *err);
EAPI Eina_Bool ender_item_attr_value_set(Ender_Item *i, void *o, Ender_Value *v, Eina_Error *err);
EAPI int ender_item_attr_flags_get(Ender_Item *i);

/**
 * @}
 */

#endif
