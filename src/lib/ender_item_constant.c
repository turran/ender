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
#define ENDER_ITEM_CONSTANT(o) (Ender_Item_Constant *)(ender_item_data_get(o))

typedef struct _Ender_Item_Constant
{
	Ender_Item *type;
	Ender_Value value;
	char *symname;
	void *sym;
} Ender_Item_Constant;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
static void _ender_item_constant_init(Ender_Item *i)
{
	i->type = ENDER_ITEM_TYPE_CONSTANT;
}

static void _ender_item_constant_deinit(Ender_Item *i)
{
	Ender_Item_Constant *thiz;

	thiz = ENDER_ITEM_CONSTANT(i);
	ender_item_unref(thiz->type);
	if (thiz->symname)
		free(thiz->symname);
	free(thiz);
}

static Ender_Item_Descriptor _descriptor = {
	/* .init 	= */ _ender_item_constant_init,
	/* .deinit 	= */ _ender_item_constant_deinit,
};

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_constant_new(void)
{
	Ender_Item *i;
	Ender_Item_Constant *thiz;

	thiz = calloc(1, sizeof(Ender_Item_Constant));
	i = ender_item_new(&_descriptor, thiz);
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
/**
 * Get the type of a constant
 * @param i The constant to get the type from
 * @return The type of a constant
 */
EAPI Ender_Item * ender_item_constant_type_get(Ender_Item *i)
{
	Ender_Item_Constant *thiz;

	thiz = ENDER_ITEM_CONSTANT(i);
	return ender_item_ref(thiz->type);
}

/**
 * Get the value of a constant
 * @param i The constant to get the value from
 * @param[out] v The value of the constant
 */
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
