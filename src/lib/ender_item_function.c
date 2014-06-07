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
#include "ender_item_function.h"

#include "ender_main_private.h"
#include "ender_item_function_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_FUNCTION(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Function, ender_item_function_descriptor_get())

typedef struct _Ender_Item_Function
{
	Ender_Item base;
	Ender_Item *ret;
	Eina_List *args;
	char *symname;
	int flags;
} Ender_Item_Function;

typedef struct _Ender_Item_Function_Class
{
	Ender_Item_Class base;
} Ender_Item_Function_Class;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENDER_ITEM_DESCRIPTOR,
		Ender_Item_Function, Ender_Item_Function_Class,
		ender_item_function);

static void _ender_item_function_class_init(void *k)
{
}

static void _ender_item_function_instance_init(void *o)
{
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_FUNCTION;
}

static void _ender_item_function_instance_deinit(void *o)
{
	Ender_Item_Function *thiz;
	Ender_Item *i;

	thiz = ENDER_ITEM_FUNCTION(o);
	ender_item_unref(thiz->ret);
	EINA_LIST_FREE(thiz->args, i)
	{
		ender_item_unref(i);
	}
	if (thiz->symname)
		free(thiz->symname);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_function_new(void)
{
	Ender_Item *i;

	i = ENESIM_OBJECT_INSTANCE_NEW(ender_item_function);
	return i;
}

void ender_item_function_symname_set(Ender_Item *i, const char *symname)
{
	Ender_Item_Function *thiz;

	thiz = ENDER_ITEM_FUNCTION(i);
	thiz->symname = strdup(symname);
}

void ender_item_function_arg_add(Ender_Item *i, Ender_Item *arg)
{
	Ender_Item_Function *thiz;
	Ender_Item_Type type;

	type = ender_item_type_get(arg);
	if (type != ENDER_ITEM_TYPE_ARG)
	{
		ender_item_unref(arg);
		return;
	}

	thiz = ENDER_ITEM_FUNCTION(i);
	thiz->args = eina_list_append(thiz->args, arg);
	ender_item_parent_set(arg, i);
}

void ender_item_function_flags_set(Ender_Item *i, int flags)
{
	Ender_Item_Function *thiz;

	thiz = ENDER_ITEM_FUNCTION(i);
	thiz->flags = flags;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_List * ender_item_function_args_get(Ender_Item *i)
{
	Ender_Item_Function *thiz;
	Eina_List *ret = NULL;
	Eina_List *l;

	thiz = ENDER_ITEM_FUNCTION(i);
	/* prepend the parent */
	if (thiz->flags & ENDER_ITEM_FUNCTION_FLAG_IS_METHOD)
		ret = eina_list_append(ret, ender_item_parent_get(i));
	EINA_LIST_FOREACH(thiz->args, l, i)
	{
		ret = eina_list_append(ret, ender_item_ref(i));
	}
	return ret;
}

EAPI int ender_item_function_args_count(Ender_Item *i)
{
	Ender_Item_Function *thiz;
	int ret;

	thiz = ENDER_ITEM_FUNCTION(i);
	ret = eina_list_count(thiz->args);
	/* for methods we pass the object as the first argument */
	if (thiz->flags & ENDER_ITEM_FUNCTION_FLAG_IS_METHOD)
		ret++;
	return ret;
}

EAPI int ender_item_function_flags_get(Ender_Item *i)
{
	Ender_Item_Function *thiz;

	thiz = ENDER_ITEM_FUNCTION(i);
	return thiz->flags;
}

EAPI Ender_Item * ender_item_function_ret_get(Ender_Item *i)
{
	Ender_Item_Function *thiz;

	thiz = ENDER_ITEM_FUNCTION(i);
	return ender_item_ref(thiz->ret);
}

Eina_Bool ender_item_function_call(Ender_Item *i, Ender_Value *args)
{
	printf("calling function %s\n", ender_item_name_get(i));
	/* TODO use FFI for this case  :)*/
	return EINA_FALSE;
}

