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
#include "ender_private.h"

#include "ender_main.h"
#include "ender_value.h"
#include "ender_item.h"
#include "ender_item_arg.h"

#include "ender_main_private.h"
#include "ender_item_arg_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_ARG(o) (Ender_Item_Arg *)(ender_item_data_get(o))

typedef struct _Ender_Item_Arg
{
	Ender_Item *type;
	Ender_Item_Transfer transfer;
	Ender_Item_Arg_Direction direction;
	int flags;
} Ender_Item_Arg;
/*----------------------------------------------------------------------------*
 *                             Item descriptor                                *
 *----------------------------------------------------------------------------*/
static void _ender_item_arg_init(Ender_Item *i)
{
	i->type = ENDER_ITEM_TYPE_ARG;
}

static void _ender_item_arg_deinit(Ender_Item *i)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	ender_item_unref(thiz->type);
	free(thiz);
}

static Ender_Item_Descriptor _descriptor = {
	/* .init 	= */ _ender_item_arg_init,
	/* .deinit 	= */ _ender_item_arg_deinit,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_arg_new(void)
{
	Ender_Item *i;
	Ender_Item_Arg *thiz;

	thiz = calloc(1, sizeof(Ender_Item_Arg));
	i = ender_item_new(&_descriptor, thiz);
	return i;
}

void ender_item_arg_type_set(Ender_Item *i, Ender_Item *t)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	if (thiz->type)
	{
		WRN("Type already set");
		ender_item_unref(t);
		return;
	}
	thiz->type = t;
}

void ender_item_arg_transfer_set(Ender_Item *i, Ender_Item_Transfer transfer)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	thiz->transfer = transfer;
}

void ender_item_arg_direction_set(Ender_Item *i, Ender_Item_Arg_Direction direction)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	thiz->direction = direction;
}

void ender_item_arg_flags_set(Ender_Item *i, int flags)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	thiz->flags = flags;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Get the type of the argument
 * @param i The argument to get the type from
 * @return The type of the argument
 */
EAPI Ender_Item * ender_item_arg_type_get(Ender_Item *i)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	return ender_item_ref(thiz->type);
}

/**
 * Get the transfer mode of an argument
 * @param i The argument to get the transfer from
 * @return The transfer mode
 */
EAPI Ender_Item_Transfer ender_item_arg_transfer_get(Ender_Item *i)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	return thiz->transfer;
}

/**
 * Get the direction of an argument
 * @param i The argument to get the direction from
 * @return The argument direction
 */
EAPI Ender_Item_Arg_Direction ender_item_arg_direction_get(Ender_Item *i)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	return thiz->direction;
}

/**
 * Get the flags of an argument
 * @param i The argument to get the flags from
 * @return The argument flags
 */
EAPI int ender_item_arg_flags_get(Ender_Item *i)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	return thiz->flags;
}
