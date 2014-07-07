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
#include "ender_item_function.h"
#include "ender_item_basic.h"
#include "ender_item_arg.h"

#include "ender_main_private.h"
#include "ender_item_attr_private.h"
#include "ender_item_struct_private.h"
#include "ender_item_object_private.h"
/*
 * TODO:
 * Add helper functions to inform about the direction of the getter/setter
 * Add helper functions to inform about the transfer of the getter/setter
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_ATTR(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Attr, ender_item_attr_descriptor_get())

typedef enum _Ender_Item_Attr_Getter_Type
{
	/* TYPE foo_get(object) */
	ENDER_ITEM_ATTR_GETTER_TYPE_RETURN,
	/* TYPE foo_get(object, error) */
	ENDER_ITEM_ATTR_GETTER_TYPE_RETURN_THROW,
	/* [bool, void] foo_get(object, TYPE) */
	ENDER_ITEM_ATTR_GETTER_TYPE_INOUT,
	/* bool foo_get(object, TYPE, error) */
	ENDER_ITEM_ATTR_GETTER_TYPE_INOUT_THROW,
} Ender_Item_Attr_Getter_Type;

typedef struct _Ender_Item_Attr
{
	Ender_Item base;
	Ender_Item *type;
	Ender_Item *setter;
	Ender_Item *getter;
	Ender_Item_Attr_Getter_Type getter_type;
	ssize_t offset;
	int flags;
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
	Ender_Item_Attr *thiz;
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_ATTR;

	thiz = ENDER_ITEM_ATTR(o);
	thiz->offset = -1;
}

static void _ender_item_attr_instance_deinit(void *o)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(o);
	ender_item_unref(thiz->type);
	if (thiz->getter)
	{
		ender_item_parent_set(thiz->getter, NULL);
		ender_item_unref(thiz->getter);
	}
	if (thiz->setter)
	{
		ender_item_parent_set(thiz->setter, NULL);
		ender_item_unref(thiz->setter);
	}
}

static Eina_Bool _ender_item_attr_getter_type_get(Ender_Item *getter,
		Ender_Item_Attr_Getter_Type *type)
{
	int nargs;

	nargs = ender_item_function_args_count(getter);
	/* TYPE foo_get(object) */
	if (nargs == 1)
	{
		*type = ENDER_ITEM_ATTR_GETTER_TYPE_RETURN;
		return EINA_TRUE;
	}
	else if (nargs == 2)
	{
		Ender_Item *ret;
		Ender_Item *arg;

		ret = ender_item_function_ret_get(getter);
		if (!ret)
		{
			/* void foo_get(object, TYPE) */
			*type = ENDER_ITEM_ATTR_GETTER_TYPE_INOUT;
			return EINA_TRUE;
		}
		else
		{
			Ender_Item *itype;

			ender_item_unref(ret);
			arg = ender_item_function_args_at(getter, 1);
			itype = ender_item_arg_type_get(arg);
			if (ender_item_is_exception(itype))
			{
				ender_item_unref(itype);
				ender_item_unref(arg);
	 			/* TYPE foo_get(object, error) */
				*type = ENDER_ITEM_ATTR_GETTER_TYPE_RETURN_THROW;
				return EINA_TRUE;
			}
			else
			{
				ender_item_unref(itype);
				ender_item_unref(arg);
	 			/* bool foo_get(object, TYPE) */
				*type = ENDER_ITEM_ATTR_GETTER_TYPE_INOUT;
				return EINA_TRUE;
			}
		}
	}
	/* bool foo_get(object, TYPE, error) */
	else if (nargs == 3)
	{
		*type = ENDER_ITEM_ATTR_GETTER_TYPE_INOUT_THROW;
		return EINA_TRUE;
	}
	else
	{
		return EINA_FALSE;
	}
}

static Eina_Bool _ender_item_attr_value_get(Ender_Item *getter,
		Ender_Item_Attr_Getter_Type type, void *o,
		Ender_Value *v, Eina_Error *err)
{
	Eina_Bool ret = EINA_FALSE;

	DBG("Getting value of getter type %d", type);
	switch (type)
	{
		case ENDER_ITEM_ATTR_GETTER_TYPE_RETURN:
		{
			Ender_Value args[1];

			args[0].ptr = o;
			ret = ender_item_function_call(getter, args, v);
		}
		break;

		case ENDER_ITEM_ATTR_GETTER_TYPE_RETURN_THROW:
		{
			Ender_Value args[2];

			args[0].ptr = o;
			args[1].ptr = err;
			ret = ender_item_function_call(getter, args, v); 
		}
		break;

		case ENDER_ITEM_ATTR_GETTER_TYPE_INOUT:
		{
			Ender_Value args[2];

			args[0].ptr = o;
			args[1] = *v;
			/* TODO check for the return value */
			ret = ender_item_function_call(getter, args, NULL); 
		}
		break;

		case ENDER_ITEM_ATTR_GETTER_TYPE_INOUT_THROW:
		{
			Ender_Value args[3];

			args[0].ptr = o;
			args[1] = *v;
			args[2].ptr = err;
			/* TODO check for the return value */
			ret = ender_item_function_call(getter, args, NULL); 
		}
		break;
	}
	return ret;
}

static Eina_Bool _ender_item_attr_value_set(Ender_Item *setter, void *o,
		Ender_Value *v, Eina_Error *err)
{
	Ender_Value args[3];
	Ender_Value ret = { 0 };
	Eina_Bool ok;

	args[0].ptr = o;
	args[1] = *v;
	args[2].ptr = err;

	ok = ender_item_function_call(setter, args, &ret);
	if (ok)
	{
		Ender_Item *rarg;
		rarg = ender_item_function_ret_get(setter);
		if (rarg)
		{
			ok = ret.b;
		}
		ender_item_unref(rarg);
	}
	return ok;
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
	ender_item_parent_set(f, i);
}

void ender_item_attr_getter_set(Ender_Item *i, Ender_Item *f)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	if (thiz->getter)
	{
		ender_item_unref(thiz->getter);
		thiz->getter = NULL;
	}
	if (!_ender_item_attr_getter_type_get(f, &thiz->getter_type))
	{
		ERR("Wrong type of getter");
		ender_item_unref(f);
		return;
	}
	thiz->getter = f;
	ender_item_parent_set(f, i);
}

void ender_item_attr_offset_set(Ender_Item *i, ssize_t offset)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	thiz->offset = offset;
}

void ender_item_attr_flags_set(Ender_Item *i, int flags)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	thiz->flags = flags;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Item * ender_item_attr_type_get(Ender_Item *i)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	if (thiz->type)
	{
		return ender_item_ref(thiz->type);
	}
	/* check if we have a getter/setter and get the type from the
	 * function types
	 */
	else
	{
		Ender_Item *arg = NULL;
		Ender_Item *ret;

		/* [bool, void] foo_set(object, TYPE, error) */
		if (thiz->setter)
		{

			arg = ender_item_function_args_at(thiz->setter, 1);
			if (!arg)
			{
				CRI("No type on setter arg[1]");
				return NULL;
			}
			ret = ender_item_arg_type_get(arg);
			ender_item_unref(arg);
			return ret;
		}
		else if (thiz->getter)
		{
			switch (thiz->getter_type)
			{
				case ENDER_ITEM_ATTR_GETTER_TYPE_RETURN:
				case ENDER_ITEM_ATTR_GETTER_TYPE_RETURN_THROW:
				arg = ender_item_function_ret_get(thiz->getter);
				break;

				case ENDER_ITEM_ATTR_GETTER_TYPE_INOUT:
				case ENDER_ITEM_ATTR_GETTER_TYPE_INOUT_THROW:
				arg = ender_item_function_args_at(thiz->getter, 1);
				break;

				default:
				break;
			}
			if (!arg)
			{
				CRI("No type on setter arg[1] or ret");
				return NULL;
			}
			ret = ender_item_arg_type_get(arg);
			ender_item_unref(arg);
			return ret;
		}
		else
		{
			CRI("No type for attr");
			return NULL;
		}
	}
	return NULL;
}

EAPI ssize_t ender_item_attr_offset_get(Ender_Item *i)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	return thiz->offset;
}

EAPI int ender_item_attr_flags_get(Ender_Item *i)
{
	Ender_Item_Attr *thiz;

	thiz = ENDER_ITEM_ATTR(i);
	return thiz->flags;
}

EAPI Eina_Bool ender_item_attr_value_get(Ender_Item *i, void *o, Ender_Value *v, Eina_Error *err)
{
	Ender_Item_Attr *thiz;
	Ender_Item *parent;
	Eina_Bool ret = EINA_FALSE;

	thiz = ENDER_ITEM_ATTR(i);
	if (thiz->getter)
		return _ender_item_attr_value_get(thiz->getter,
				thiz->getter_type, o, v, err);

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
	Ender_Item_Attr *thiz;
	Ender_Item *parent;
	Eina_Bool ret = EINA_FALSE;

	thiz = ENDER_ITEM_ATTR(i);
	if (thiz->getter)
		return _ender_item_attr_value_set(thiz->setter, o, v, err);

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

