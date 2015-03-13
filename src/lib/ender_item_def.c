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
#include "ender_item_def.h"

#include "ender_main_private.h"
#include "ender_item_def_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_DEF(o) (Ender_Item_Def *)(ender_item_data_get(o))

typedef struct _Ender_Item_Def
{
	Ender_Item *type;
	Eina_List *functions;
} Ender_Item_Def;

/*----------------------------------------------------------------------------*
 *                             Item descriptor                                *
 *----------------------------------------------------------------------------*/
static void _ender_item_def_init(Ender_Item *i)
{
	i->type = ENDER_ITEM_TYPE_DEF;
}

static void _ender_item_def_deinit(Ender_Item *i)
{
	Ender_Item_Def *thiz;

	thiz = ENDER_ITEM_DEF(i);
	ender_item_unref(thiz->type);
	EINA_LIST_FREE(thiz->functions, i)
	{
		ender_item_parent_set(i, NULL);
		ender_item_unref(i);
	}
	free(thiz);
}

static Ender_Item_Descriptor _descriptor = {
	/* .init 	= */ _ender_item_def_init,
	/* .deinit 	= */ _ender_item_def_deinit,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_def_new(void)
{
	Ender_Item *i;
	Ender_Item_Def *thiz;

	thiz = calloc(1, sizeof(Ender_Item_Def));
	i = ender_item_new(&_descriptor, thiz);
	return i;
}

void ender_item_def_type_set(Ender_Item *i, Ender_Item *t)
{
	Ender_Item_Def *thiz;

	thiz = ENDER_ITEM_DEF(i);
	if (thiz->type)
	{
		WRN("Type already set");
		ender_item_unref(t);
		return;
	}
	thiz->type = t;
}

void ender_item_def_function_add(Ender_Item *i, Ender_Item *f)
{
	Ender_Item_Def *thiz;
	Ender_Item_Type type;

	type = ender_item_type_get(f);
	if (type != ENDER_ITEM_TYPE_FUNCTION)
	{
		ender_item_unref(f);
		return;
	}
		
	thiz = ENDER_ITEM_DEF(i);
	thiz->functions = eina_list_append(thiz->functions, f);
	ender_item_parent_set(f, i);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Get the type of a def
 * @param i The def to get the type from
 * @return The type of a def
 */
EAPI Ender_Item * ender_item_def_type_get(Ender_Item *i)
{
	Ender_Item_Def *thiz;

	thiz = ENDER_ITEM_DEF(i);
	return ender_item_ref(thiz->type);
}

/**
 * Get the functions of a def
 * @param i The def to get the functions from
 * @return The list of functions. Free it by calling @ref ender_item_unref on
 * each item. Each item is of type @ref ENDER_ITEM_TYPE_FUNCTION.
 */
EAPI Eina_List * ender_item_def_functions_get(Ender_Item *i)
{
	Ender_Item_Def *thiz;
	Ender_Item *f;
	Eina_List *ret = NULL;
	Eina_List *l;

	thiz = ENDER_ITEM_DEF(i);
	EINA_LIST_FOREACH(thiz->functions, l, f)
	{
		ret = eina_list_append(ret, ender_item_ref(f));
	}
	return ret;
}
