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
#include "ender_item_enum.h"

#include "ender_main_private.h"
#include "ender_item_enum_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_ENUM(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Enum, ender_item_enum_descriptor_get())

typedef struct _Ender_Item_Enum
{
	Ender_Item base;
	Eina_List *values;
} Ender_Item_Enum;

typedef struct _Ender_Item_Enum_Class
{
	Ender_Item_Class base;
} Ender_Item_Enum_Class;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENDER_ITEM_DESCRIPTOR,
		Ender_Item_Enum, Ender_Item_Enum_Class,
		ender_item_enum);

static void _ender_item_enum_class_init(void *k)
{
}

static void _ender_item_enum_instance_init(void *o)
{
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_ENUM;
}

static void _ender_item_enum_instance_deinit(void *o)
{
	Ender_Item_Enum *thiz;
	Ender_Item *c;

	thiz = ENDER_ITEM_ENUM(o);
	EINA_LIST_FREE(thiz->values, c)
	{
		ender_item_unref(c);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_enum_new(void)
{
	Ender_Item *i;

	i = ENESIM_OBJECT_INSTANCE_NEW(ender_item_enum);
	return i;
}

void ender_item_enum_value_add(Ender_Item *i, Ender_Item *value)
{
	Ender_Item_Enum *thiz;
	Ender_Item_Type type;

	if (!value) return;

	type = ender_item_type_get(value);
	if (type != ENDER_ITEM_TYPE_CONSTANT)
	{
		CRI("Unsupported type '%d'", type);
		ender_item_unref(value);
		return;
	}

	thiz = ENDER_ITEM_ENUM(i);
	thiz->values = eina_list_append(thiz->values, value);
	ender_item_parent_set(value, i);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_List * ender_item_enum_values_get(Ender_Item *i)
{
	Ender_Item_Enum *thiz;
	Eina_List *ret = NULL;
	Eina_List *l;

	thiz = ENDER_ITEM_ENUM(i);
	EINA_LIST_FOREACH(thiz->values, l, i)
	{
		ret = eina_list_append(ret, ender_item_ref(i));
	}
	return ret;
}
