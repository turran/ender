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

/**
 * @defgroup Ender_Arg_Group Arg
 * @brief An arg represents a @ref Ender_Function_Group argument
 * @{
 */

/**
 * The direction of an argument
 * @see ender_item_arg_direction_get
 */
typedef enum _Ender_Item_Arg_Direction
{
	/**
	 * Is an IN argument. @ref Ender_Struct_Group are always passed
	 * by reference
	 */
	ENDER_ITEM_ARG_DIRECTION_IN,
	/**
	 * Is an OUT argument, for every item that has known memory layout
	 * (like a @ref Ender_Struct_Group or a @ref Ender_Basic_Group) the
	 * expected argument is a pointer to the item C type. For unknown
	 * memory layout types (like a @ref Ender_Object_Group) it is a double
	 * pointer */
	ENDER_ITEM_ARG_DIRECTION_OUT,
	/**
	 * Both @ref ENDER_ITEM_ARG_DIRECTION_IN and
	 * @ref ENDER_ITEM_ARG_DIRECTION_OUT
	 */
	ENDER_ITEM_ARG_DIRECTION_IN_OUT,
} Ender_Item_Arg_Direction;

/**
 * Different flags an argument can have
 */
typedef enum _Ender_Item_Arg_Flag
{
	/**
	 * In case the argument is the return value
	 * @see ender_item_function_ret_get
	 */
	ENDER_ITEM_ARG_FLAG_IS_RETURN          = (1 << 0),
	/** In a callback, this argument is the user provided data */
	ENDER_ITEM_ARG_FLAG_IS_CLOSURE         = (1 << 1),
	/** In case the argument can be NULL */
	ENDER_ITEM_ARG_FLAG_NULLABLE           = (1 << 2),
	/** In case the argument type is a callback, inform that the callback
	 * will be called later, if this flag is not set, the callback is
	 * assumed to be called immediately
	 */
	ENDER_ITEM_ARG_FLAG_DELAYED_CALLBACK   = (1 << 4)
} Ender_Item_Arg_Flag;

EAPI Ender_Item * ender_item_arg_type_get(Ender_Item *i);
EAPI Ender_Item_Arg_Direction ender_item_arg_direction_get(Ender_Item *i);
EAPI Ender_Item_Transfer ender_item_arg_transfer_get(Ender_Item *i);
EAPI int ender_item_arg_flags_get(Ender_Item *i);

/**
 * @}
 */

#endif
