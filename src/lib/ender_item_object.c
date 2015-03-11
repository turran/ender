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
#include "ender_item_function.h"

#include "ender_main_private.h"
#include "ender_item_attr_private.h"
#include "ender_item_object_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_OBJECT(o) (Ender_Item_Object *)(ender_item_data_get(o))

typedef struct _Ender_Item_Object
{
	Ender_Item *inherit;
	Ender_Item *ref;
	Ender_Item *unref;
	Eina_List *functions;
	Eina_List *props;
} Ender_Item_Object;

static void _ender_item_object_function_add(Ender_Item *i,
		Ender_Item *f)
{
	Ender_Item_Object *thiz;

	thiz = ENDER_ITEM_OBJECT(i);
	thiz->functions = eina_list_append(thiz->functions, f);
	ender_item_parent_set(f, i);
}

/*----------------------------------------------------------------------------*
 *                             Item descriptor                                *
 *----------------------------------------------------------------------------*/
static void _ender_item_object_init(Ender_Item *i)
{
	i->type = ENDER_ITEM_TYPE_OBJECT;
}

static void _ender_item_object_deinit(Ender_Item *i)
{
	Ender_Item_Object *thiz;

	thiz = ENDER_ITEM_OBJECT(i);
	EINA_LIST_FREE(thiz->functions, i)
	{
		ender_item_parent_set(i, NULL);
		ender_item_unref(i);
	}
	EINA_LIST_FREE(thiz->props, i)
	{
		ender_item_parent_set(i, NULL);
		ender_item_unref(i);
	}
	if (thiz->inherit)
	{
		ender_item_parent_set(thiz->inherit, NULL);
		ender_item_unref(thiz->inherit);
	}
	if (thiz->ref)
	{
		ender_item_parent_set(thiz->ref, NULL);
		ender_item_unref(thiz->ref);
	}
	if (thiz->unref)
	{
		ender_item_parent_set(thiz->unref, NULL);
		ender_item_unref(thiz->unref);
	}
	free(thiz);
}

static Ender_Item_Descriptor _descriptor = {
	/* .init 	= */ _ender_item_object_init,
	/* .deinit 	= */ _ender_item_object_deinit,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_object_new(void)
{
	Ender_Item *i;
	Ender_Item_Object *thiz;

	thiz = calloc(1, sizeof(Ender_Item_Object));
	i = ender_item_new(&_descriptor, thiz);
	return i;
}

void ender_item_object_inherit_set(Ender_Item *i, Ender_Item *in)
{
	Ender_Item_Object *thiz;

	thiz = ENDER_ITEM_OBJECT(i);
	if (thiz->inherit)
	{
		WRN("Inherit already set");
		ender_item_unref(in);
		return;
	}
	thiz->inherit = in;
}

void ender_item_object_function_add(Ender_Item *i, Ender_Item *f)
{
	Ender_Item_Object *thiz;
	Ender_Item_Type type;
	int flags;

	thiz = ENDER_ITEM_OBJECT(i);
	type = ender_item_type_get(f);
	if (type != ENDER_ITEM_TYPE_FUNCTION)
	{
		ender_item_unref(f);
		return;
	}

	flags = ender_item_function_flags_get(f);
	if (flags & ENDER_ITEM_FUNCTION_FLAG_UNREF)
	{
		if (!thiz->unref)
			thiz->unref = ender_item_ref(f);
		else
		{
			WRN("Unref function already set");
			ender_item_unref(f);
			return;
		}
	}

	if (flags & ENDER_ITEM_FUNCTION_FLAG_REF)
	{
		if (!thiz->ref)
			thiz->ref = ender_item_ref(f);
		else
		{
			WRN("Ref function already set");
			ender_item_unref(f);
			return;
		}
	}

	_ender_item_object_function_add(i, f);
}

void ender_item_object_prop_add(Ender_Item *i, Ender_Item *p)
{
	Ender_Item_Object *thiz;

	thiz = ENDER_ITEM_OBJECT(i);
	if (ender_item_type_get(p) != ENDER_ITEM_TYPE_ATTR)
	{
		ERR("Type must be an attribute");
		ender_item_unref(p);
		return;
	}
	thiz->props = eina_list_append(thiz->props, p);
	ender_item_parent_set(p, i);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool ender_item_object_string_to(Ender_Item *i, void *o, char **str,
		Ender_Item_Transfer *xfer, Eina_Error *err)
{
	Ender_Item_Object *thiz;
	Ender_Item *item;
	Ender_Value val = { 0 };
	Eina_List *l;
	Eina_Bool ret = EINA_FALSE;
	int flags;

	thiz = ENDER_ITEM_OBJECT(i);
	/* check for attributes that have the value-of flag */
	EINA_LIST_FOREACH (thiz->props, l, item)
	{
		flags = ender_item_attr_flags_get(item);
		if (flags & ENDER_ITEM_ATTR_FLAG_VALUE_OF)
		{
			DBG("Property found '%s'", ender_item_name_get(item));
			ret = ender_item_attr_value_get(item, o, xfer, &val, err);
			*str = val.ptr;
			goto done;
		}
	}
	/* check for functions that have the value-of flag */
	EINA_LIST_FOREACH (thiz->functions, l, item)
	{
		flags = ender_item_function_flags_get(item);
		if (flags & ENDER_ITEM_FUNCTION_FLAG_VALUE_OF)
		{
			DBG("Function found '%s'", ender_item_name_get(item));
			break;
		}
	}
done:
	return ret;
}

EAPI Ender_Item * ender_item_object_downcast(Ender_Item *i, void *o)
{
	Ender_Item_Object *thiz;
	Ender_Item *item;
	Ender_Item *ret = NULL;
	Eina_List *l;
	Eina_Bool found = EINA_FALSE;

	thiz = ENDER_ITEM_OBJECT(i);
	/* check for attributes that have the downcast flag */
	EINA_LIST_FOREACH (thiz->props, l, item)
	{
		Ender_Value val = { 0 };
		int flags;

		flags = ender_item_attr_flags_get(item);
		if (flags & ENDER_ITEM_ATTR_FLAG_DOWNCAST)
		{
			DBG("Property found '%s'", ender_item_name_get(item));
			ender_item_attr_value_get(item, o, NULL, &val, NULL);
			ret = val.ptr;
			found = EINA_TRUE;
			goto done;
		}
	}
done:
	/* it might be possible that we are still an abstract class in the middle
	 * of the hierarchy, for that, go up until no inherits
	 */
	if (!found)
	{
		Ender_Item *inherit;

		inherit = ender_item_object_inherit_get(i);
		if (inherit)
		{
			ret = ender_item_object_downcast(inherit, o);
			ender_item_unref(inherit);
		}
	}
	return ret;
}

EAPI Ender_Item * ender_item_object_inherit_get(Ender_Item *i)
{
	Ender_Item_Object *thiz;

	thiz = ENDER_ITEM_OBJECT(i);
	return ender_item_ref(thiz->inherit);
}

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

EAPI Eina_List * ender_item_object_props_get(Ender_Item *i)
{
	Ender_Item_Object *thiz;
	Ender_Item *p;
	Eina_List *ret = NULL;
	Eina_List *l;

	thiz = ENDER_ITEM_OBJECT(i);
	EINA_LIST_FOREACH(thiz->props, l, p)
	{
		ret = eina_list_append(ret, ender_item_ref(p));
	}
	return ret;
}

EAPI Eina_List * ender_item_object_ctor_get(Ender_Item *i)
{
	Ender_Item_Object *thiz;
	Ender_Item *f;
	Eina_List *l;
	Eina_List *ret = NULL;

	thiz = ENDER_ITEM_OBJECT(i);
	EINA_LIST_FOREACH(thiz->functions, l, f)
	{
		int flags;

		flags = ender_item_function_flags_get(f);
		if (flags & ENDER_ITEM_FUNCTION_FLAG_CTOR)
			ret = eina_list_append(ret, ender_item_ref(f));
	}
	return ret;
}

EAPI Eina_Bool ender_item_object_ref(Ender_Item *i, void *o)
{
	Ender_Item_Object *thiz;
	Ender_Value args[2];

	thiz = ENDER_ITEM_OBJECT(i);
	if (!thiz->ref)
	{
		if (!thiz->inherit)
			return EINA_FALSE;
		return ender_item_object_ref(thiz->inherit, o);
	}
	args[0].ptr = o;
	return ender_item_function_call(thiz->ref, args, NULL);
}

EAPI Eina_Bool ender_item_object_unref(Ender_Item *i, void *o)
{
	Ender_Item_Object *thiz;
	Ender_Value args[1];

	thiz = ENDER_ITEM_OBJECT(i);
	if (!thiz->unref)
	{
		if (!thiz->inherit)
			return EINA_FALSE;
		return ender_item_object_unref(thiz->inherit, o);
	}
	args[0].ptr = o;
	return ender_item_function_call(thiz->unref, args, NULL);
}
