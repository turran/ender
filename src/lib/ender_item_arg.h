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

#ifndef _ENDER_ITEM_ARG_H_
#define _ENDER_ITEM_ARG_H_

typedef enum _Ender_Transfer_Type
{
	ENDER_TRANSFER_TYPE_FULL,
	ENDER_TRANSFER_TYPE_NONE,
	ENDER_TRANSFER_TYPE_CONTAINER,
	ENDER_TRANSFER_TYPE_CONTENT,
} Ender_Transfer_Type;

typedef enum _Ender_Item_Arg_Direction
{
	ENDER_ITEM_ARG_DIRECTION_IN,
	ENDER_ITEM_ARG_DIRECTION_OUT,
	ENDER_ITEM_ARG_DIRECTION_IN_OUT,
} Ender_Item_Arg_Direction;

typedef enum _Ender_Item_Arg_Flags
{
	ENDER_ITEM_ARG_FLAG_IS_RETURN = (1 << 0),
} Ender_Item_Arg_Flags;

EAPI Ender_Item * ender_item_arg_type_get(Ender_Item *i);
EAPI Ender_Item_Arg_Direction ender_item_arg_direction_get(Ender_Item *i);
EAPI Ender_Transfer_Type ender_item_arg_transfer_get(Ender_Item *i);
EAPI int ender_item_arg_flags_get(Ender_Item *i);

#endif
