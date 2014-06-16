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
#include "ender_item_basic.h"
#include "ender_item_arg.h"

#include "ender_main_private.h"
#include "ender_item_private.h"
#include "ender_item_function_private.h"
#include "ender_item_arg_private.h"
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
	void *sym;
} Ender_Item_Function;

typedef struct _Ender_Item_Function_Class
{
	Ender_Item_Class base;
} Ender_Item_Function_Class;

/* TODO handle the direction */
static ffi_type * _ender_item_function_arg_ffi_to(Ender_Item *i)
{
	Ender_Item_Type type;

	if (!i)
	{
		WRN("No item found");
		return &ffi_type_pointer;
	}

	type = ender_item_type_get(i);
	switch (type)
	{
		case ENDER_ITEM_TYPE_BASIC:
		{
			Ender_Value_Type vt;

			vt = ender_item_basic_value_type_get(i);
			switch (vt)
			{
				case ENDER_VALUE_TYPE_BOOL:
				return &ffi_type_uint8;
				break;

				case ENDER_VALUE_TYPE_UINT8:
				return &ffi_type_uint8;
				break;

				case ENDER_VALUE_TYPE_INT8:
				return &ffi_type_sint8;
				break;

				case ENDER_VALUE_TYPE_UINT32:
				return &ffi_type_uint32;
				break;

				case ENDER_VALUE_TYPE_INT32:
				return &ffi_type_sint32;
				break;

				case ENDER_VALUE_TYPE_UINT64:
				return &ffi_type_uint64;
				break;

				case ENDER_VALUE_TYPE_INT64:
				return &ffi_type_sint64;
				break;

				case ENDER_VALUE_TYPE_DOUBLE:
				return &ffi_type_double;
				break;

				case ENDER_VALUE_TYPE_STRING:
				case ENDER_VALUE_TYPE_POINTER:
				return &ffi_type_pointer;
				break;

				default:
				ERR("Unsupported item type '%d'", vt);
				return &ffi_type_pointer;
				break;
			}
		}
		break;

		case ENDER_ITEM_TYPE_ENUM:
		return &ffi_type_sint32;
		break;

		case ENDER_ITEM_TYPE_STRUCT:
		case ENDER_ITEM_TYPE_FUNCTION:
		case ENDER_ITEM_TYPE_OBJECT:
		return &ffi_type_pointer;
		break;

		default:
		ERR("Unsupported item type '%d'", type);
		return &ffi_type_pointer;
		break;
	}
	
}

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
	if (thiz->symname)
	{
		WRN("Symname already set");
		return;
	}
	thiz->symname = strdup(symname);
}

void ender_item_function_ret_set(Ender_Item *i, Ender_Item *arg)
{
	Ender_Item_Function *thiz;
	Ender_Item_Type type;

	type = ender_item_type_get(arg);
	if (type != ENDER_ITEM_TYPE_ARG)
	{
		CRI("Unsupported type '%d'", type);
		ender_item_unref(arg);
		return;
	}
	thiz = ENDER_ITEM_FUNCTION(i);
	thiz->ret = arg;
	ender_item_parent_set(arg, i);
}

void ender_item_function_arg_add(Ender_Item *i, Ender_Item *arg)
{
	Ender_Item_Function *thiz;
	Ender_Item_Type type;

	if (!arg) return;

	type = ender_item_type_get(arg);
	if (type != ENDER_ITEM_TYPE_ARG)
	{
		CRI("Unsupported type '%d'", type);
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
	if (thiz->flags & ENDER_ITEM_FUNCTION_FLAG_IS_METHOD)
	{
		Ender_Item *arg;

		arg = ender_item_arg_new();
		ender_item_name_set(arg, "self");
		ender_item_arg_type_set(arg, ender_item_parent_get(i));
		ret = eina_list_append(ret, arg);
	}

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
	if (thiz->flags & ENDER_ITEM_FUNCTION_FLAG_CTOR)
	{
		Ender_Item *ret;

		/* create our own arg based on the parent type */
		ret = ender_item_arg_new();
		ender_item_arg_direction_set(ret, ENDER_ITEM_ARG_DIRECTION_IN);
		ender_item_arg_transfer_set(ret, ENDER_TRANSFER_TYPE_FULL);
		ender_item_arg_type_set(ret, ender_item_parent_get(i));
		return ret;
	}
	else
	{
		return ender_item_ref(thiz->ret);
	}
}

EAPI Eina_Bool ender_item_function_call(Ender_Item *i, Ender_Value *args, Ender_Value *retval)
{
	Ender_Item_Function *thiz;
	Ender_Item *a;
	Ender_Item *i_ret;
	Eina_List *l;
	ffi_type **ffi_args;
	ffi_type *ffi_ret = &ffi_type_void;
	ffi_status status;
	ffi_cif cif;
	void **ffi_values;
	int ffi_arg = 0;
	int arg = 0;
	int nargs;

	thiz = ENDER_ITEM_FUNCTION(i);

	/* load the symbol */
	if (!thiz->sym)
	{
		DBG("Loading symbol '%s'", thiz->symname);
		thiz->sym = ender_item_sym_get(i, thiz->symname);
	}
	if (!thiz->sym)
	{
		CRI("Impossible to load the symbol '%s'", thiz->symname);
		return EINA_FALSE;
	}

	/* pass the args as ffi args */
	nargs = eina_list_count(thiz->args);
	if (thiz->flags & ENDER_ITEM_FUNCTION_FLAG_IS_METHOD)
		nargs++;

	ffi_args = calloc(nargs, sizeof(ffi_type *));
	ffi_values = calloc(nargs, sizeof(void *));

	/* fill in the args */
	if (thiz->flags & ENDER_ITEM_FUNCTION_FLAG_IS_METHOD)
	{
		ffi_args[ffi_arg] = &ffi_type_pointer;
		ffi_values[ffi_arg] = &args[arg];
		arg++;
		ffi_arg++;
	}

	EINA_LIST_FOREACH(thiz->args, l, a)
	{
		Ender_Item *type;

		type = ender_item_arg_type_get(a);
		ffi_args[ffi_arg] = _ender_item_function_arg_ffi_to(type);
		ffi_values[ffi_arg] = &args[arg];
		arg++;
		ffi_arg++;
		ender_item_unref(type);
	}

	i_ret = ender_item_function_ret_get(i);
	if (i_ret)
	{
		Ender_Item *type;

		type = ender_item_arg_type_get(i_ret);
		ffi_ret = _ender_item_function_arg_ffi_to(type);
		ender_item_unref(type);
		ender_item_unref(i_ret);
	}

	if ((status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, ffi_ret,
			ffi_args)) != FFI_OK)
	{
		/* TODO handle the ffi_status */
		ERR("FFI error");
	}

	ffi_call(&cif, FFI_FN(thiz->sym), retval, ffi_values);

	return EINA_TRUE;
}

