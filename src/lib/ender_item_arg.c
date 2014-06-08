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
#define ENDER_ITEM_ARG(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Arg, ender_item_arg_descriptor_get())

typedef struct _Ender_Item_Arg
{
	Ender_Item base;
	Ender_Item *type;
	Ender_Transfer_Type transfer;
	Ender_Item_Arg_Direction direction;
} Ender_Item_Arg;

typedef struct _Ender_Item_Arg_Class
{
	Ender_Item_Class base;
} Ender_Item_Arg_Class;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENDER_ITEM_DESCRIPTOR,
		Ender_Item_Arg, Ender_Item_Arg_Class,
		ender_item_arg);

static void _ender_item_arg_class_init(void *k)
{
}

static void _ender_item_arg_instance_init(void *o)
{
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_ARG;
}

static void _ender_item_arg_instance_deinit(void *o)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(o);
	ender_item_unref(thiz->type);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_arg_new(void)
{
	Ender_Item *i;

	i = ENESIM_OBJECT_INSTANCE_NEW(ender_item_arg);
	return i;
}

void ender_item_arg_type_set(Ender_Item *i, Ender_Item *t)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	if (thiz->type)
		ender_item_unref(thiz->type);
	thiz->type = t;
}

void ender_item_arg_transfer_set(Ender_Item *i, Ender_Transfer_Type transfer)
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
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Item * ender_item_arg_type_get(Ender_Item *i)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	return ender_item_ref(thiz->type);
}

EAPI Ender_Transfer_Type ender_item_arg_transfer_get(Ender_Item *i)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	return thiz->transfer;
}

EAPI Ender_Item_Arg_Direction ender_item_arg_direction_get(Ender_Item *i)
{
	Ender_Item_Arg *thiz;

	thiz = ENDER_ITEM_ARG(i);
	return thiz->direction;
}
