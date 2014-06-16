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
#include "ender_item_attr.h"

#include "ender_main_private.h"
#include "ender_item_attr_private.h"
#include "ender_item_struct_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_ATTR(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Attr, ender_item_attr_descriptor_get())

typedef struct _Ender_Item_Attr
{
	Ender_Item base;
	Ender_Item *type;
	Ender_Item *setter;
	Ender_Item *getter;
	ssize_t offset;
} Ender_Item_Attr;

typedef struct _Ender_Item_Attr_Class
{
	Ender_Item_Class base;
} Ender_Item_Attr_Class;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENDER_ITEM_DESCRIPTOR,
		Ender_Item_Attr, Ender_Item_Attr_Class,
		ender_item_attr);

static void _ender_item_attr_class_init(void *k)
{
}

static void _ender_item_attr_instance_init(void *o)
{
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_ATTR;
}

static void _ender_item_attr_instance_deinit(void *o)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(o);
	ender_item_unref(thiz->type);
	ender_item_unref(thiz->getter);
	ender_item_unref(thiz->setter);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_attr_new(void)
{
	Ender_Item *i;

	i = ENESIM_OBJECT_INSTANCE_NEW(ender_item_attr);
	return i;
}

void ender_item_attr_type_set(Ender_Item *i, Ender_Item *t)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	if (thiz->type)
		ender_item_unref(thiz->type);
	thiz->type = t;
}

void ender_item_attr_setter_set(Ender_Item *i, Ender_Item *f)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	if (thiz->setter)
		ender_item_unref(thiz->setter);
	thiz->setter = f;
}

void ender_item_attr_getter_set(Ender_Item *i, Ender_Item *f)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	if (thiz->getter)
		ender_item_unref(thiz->getter);
	thiz->getter = f;
}

void ender_item_attr_offset_set(Ender_Item *i, ssize_t offset)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	thiz->offset = offset;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Item * ender_item_attr_type_get(Ender_Item *i)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	return ender_item_ref(thiz->type);
}

EAPI ssize_t ender_item_attr_offset_get(Ender_Item *i)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	return thiz->offset;
}

EAPI Eina_Bool ender_item_attr_value_get(Ender_Item *i, void *o, Ender_Value *v, Eina_Error *err)
{
	Ender_Item *parent;
	Eina_Bool ret = EINA_FALSE;

	parent = ender_item_parent_get(i);
	if (!parent)
	{
		ERR("Attr without parent");
		goto done;
	}
	switch (ender_item_type_get(parent))
	{
		case ENDER_ITEM_TYPE_STRUCT:
		ret = ender_item_struct_field_value_get(o, i, v, err);
		break;

		default:
		ERR("Unsupported parent type");
		break;
	}
	ender_item_unref(parent);
done:
	return ret;
}

EAPI Eina_Bool ender_item_attr_value_set(Ender_Item *i, void *o, Ender_Value *v, Eina_Error *err)
{
	Ender_Item *parent;
	Eina_Bool ret = EINA_FALSE;

	parent = ender_item_parent_get(i);
	if (!parent)
	{
		ERR("Attr without parent");
		goto done;
	}
	switch (ender_item_type_get(parent))
	{
		case ENDER_ITEM_TYPE_STRUCT:
		ret = ender_item_struct_field_value_set(o, i, v, err);
		break;

		default:
		ERR("Unsupported parent type");
		break;
	}
	ender_item_unref(parent);
done:
	return ret;
}

