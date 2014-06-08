/* ENDER - Enesim's descriptor library
 * Copyright (C) 2010 Jorge Luis Zapata
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
#include "ender_item_object.h"
#include "ender_item_attr.h"

#include "ender_main_private.h"
#include "ender_item_attr_private.h"
#include "ender_item_object_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_OBJECT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Object, ender_item_object_descriptor_get())

typedef struct _Ender_Item_Object
{
	Ender_Item base;
	Eina_List *functions;
} Ender_Item_Object;

typedef struct _Ender_Item_Object_Object
{
	Ender_Item_Object base;
} Ender_Item_Object_Object;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENDER_ITEM_DESCRIPTOR,
		Ender_Item_Object, Ender_Item_Object_Object,
		ender_item_object);

static void _ender_item_object_class_init(void *k)
{
}

static void _ender_item_object_instance_init(void *o)
{
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_OBJECT;
}

static void _ender_item_object_instance_deinit(void *o)
{
	Ender_Item_Object *thiz;
	Ender_Item *f;
	Eina_List *l;

	thiz = ENDER_ITEM_OBJECT(o);
	EINA_LIST_FOREACH(thiz->functions, l, f)
	{
		ender_item_unref(f);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_object_new(void)
{
	Ender_Item *i;

	i = ENESIM_OBJECT_INSTANCE_NEW(ender_item_object);
	return i;
}

void ender_item_object_function_add(Ender_Item *i, Ender_Item *f)
{
	Ender_Item_Object *thiz;
	Ender_Item_Type type;

	type = ender_item_type_get(f);
	if (type != ENDER_ITEM_TYPE_FUNCTION)
	{
		ender_item_unref(f);
		return;
	}
		
	thiz = ENDER_ITEM_OBJECT(i);
	thiz->functions = eina_list_append(thiz->functions, f);
	ender_item_parent_set(f, i);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_List * ender_item_object_functions_get(Ender_Item *i)
{
	Ender_Item_Object *thiz;
	Ender_Item *f;
	Eina_List *ret = NULL;
	Eina_List *l;

	thiz = ENDER_ITEM_OBJECT(i);
	EINA_LIST_FOREACH(thiz->functions, l, f)
	{
		ret = eina_list_append(ret, ender_item_ref(f));
	}
	return ret;
}
