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
#define ENDER_ITEM_ENUM(o) (Ender_Item_Enum *)(ender_item_data_get(o))

typedef struct _Ender_Item_Enum
{
	Eina_List *values;
} Ender_Item_Enum;

/*----------------------------------------------------------------------------*
 *                             Item descriptor                                *
 *----------------------------------------------------------------------------*/
static void _ender_item_enum_init(Ender_Item *i)
{
	i->type = ENDER_ITEM_TYPE_ENUM;
}

static void _ender_item_enum_deinit(Ender_Item *i)
{
	Ender_Item_Enum *thiz;
	Ender_Item *c;

	thiz = ENDER_ITEM_ENUM(i);
	EINA_LIST_FREE(thiz->values, c)
	{
		ender_item_parent_set(c, NULL);
		ender_item_unref(c);
	}
	free(thiz);
}

static Ender_Item_Descriptor _descriptor = {
	/* .init 	= */ _ender_item_enum_init,
	/* .deinit 	= */ _ender_item_enum_deinit,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_enum_new(void)
{
	Ender_Item *i;
	Ender_Item_Enum *thiz;

	thiz = calloc(1, sizeof(Ender_Item_Enum));
	i = ender_item_new(&_descriptor, thiz);
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
/**
 * Get the values of an enum
 * @param i The enum to get the values from
 * @return The list of values. Free it by calling @ref ender_item_unref on
 * each item. Each item is of type @ref ENDER_ITEM_TYPE_CONSTANT.
 */
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
