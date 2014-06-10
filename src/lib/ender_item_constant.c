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
#include "ender_item_constant.h"

#include "ender_main_private.h"
#include "ender_item_constant_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_CONSTANT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Constant, ender_item_constant_descriptor_get())

typedef struct _Ender_Item_Constant
{
	Ender_Item base;
	Ender_Item *type;
	Ender_Value value;
	char *symname;
	void *sym;
} Ender_Item_Constant;

typedef struct _Ender_Item_Constant_Class
{
	Ender_Item_Class base;
} Ender_Item_Constant_Class;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENDER_ITEM_DESCRIPTOR,
		Ender_Item_Constant, Ender_Item_Constant_Class,
		ender_item_constant);

static void _ender_item_constant_class_init(void *k)
{
}

static void _ender_item_constant_instance_init(void *o)
{
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_CONSTANT;
}

static void _ender_item_constant_instance_deinit(void *o)
{
	Ender_Item_Constant *thiz;

	thiz = ENDER_ITEM_CONSTANT(o);
	ender_item_unref(thiz->type);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_constant_new(void)
{
	Ender_Item *i;

	i = ENESIM_OBJECT_INSTANCE_NEW(ender_item_constant);
	return i;
}

void ender_item_constant_symname_set(Ender_Item *i, const char *symname)
{
	Ender_Item_Constant *thiz;

	thiz = ENDER_ITEM_CONSTANT(i);
	if (thiz->symname)
	{
		WRN("Symname already set");
		return;
	}
	thiz->symname = strdup(symname);
}

void ender_item_constant_value_set(Ender_Item *i, Ender_Value *value)
{
	Ender_Item_Constant *thiz;
	Ender_Item_Type type;

	thiz = ENDER_ITEM_CONSTANT(i);
	if (!value)
	{
		WRN("No value to set");
		return;
	}
	thiz->value = *value;
}

void ender_item_constant_type_set(Ender_Item *i, Ender_Item *t)
{
	Ender_Item_Constant *thiz;

	thiz = ENDER_ITEM_CONSTANT(i);
	if (thiz->type)
	{
		WRN("Type already set");
		ender_item_unref(t);
		return;
	}
	thiz->type = t;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Item * ender_item_constant_type_get(Ender_Item *i)
{
	Ender_Item_Constant *thiz;

	thiz = ENDER_ITEM_CONSTANT(i);
	return ender_item_ref(thiz->type);
}

EAPI void ender_item_constant_value_get(Ender_Item *i, Ender_Value *v)
{
	Ender_Item_Constant *thiz;

	thiz = ENDER_ITEM_CONSTANT(i);
	/* in case the constant is a symbol, load it */
	if (thiz->symname && !thiz->sym)
	{
		thiz->sym = ender_item_sym_get(i, thiz->symname);
		/* convert the symbol to a value */
		WRN("Get the real value");
	}
	*v = thiz->value;
}
