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
#include "ender_item_struct.h"
#include "ender_item_attr.h"
#include "ender_item_basic.h"
#include "ender_item_def.h"

#include "ender_main_private.h"
#include "ender_value_private.h"
#include "ender_item_attr_private.h"
#include "ender_item_struct_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_STRUCT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Struct, ender_item_struct_descriptor_get())

typedef struct _Ender_Item_Struct
{
	Ender_Item base;
	Eina_List *fields;
	Eina_List *functions;
	size_t size;
} Ender_Item_Struct;

typedef struct _Ender_Item_Struct_Class
{
	Ender_Item_Class base;
} Ender_Item_Struct_Class;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENDER_ITEM_DESCRIPTOR,
		Ender_Item_Struct, Ender_Item_Struct_Class,
		ender_item_struct);

static void _ender_item_struct_class_init(void *k)
{
}

static void _ender_item_struct_instance_init(void *o)
{
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_STRUCT;
}

static void _ender_item_struct_instance_deinit(void *o)
{
	Ender_Item_Struct *thiz;
	Ender_Item *f;
	Eina_List *l;

	thiz = ENDER_ITEM_STRUCT(o);
	EINA_LIST_FOREACH(thiz->fields, l, f)
	{
		ender_item_unref(f);
	}
	EINA_LIST_FOREACH(thiz->functions, l, f)
	{
		ender_item_unref(f);
	}
}

static Eina_Bool _ender_item_field_size_alignment_get(Ender_Item *i, size_t *sz, ssize_t *al)
{
	Ender_Item_Type type;
	Ender_Item *other;
	Eina_Bool ret = EINA_FALSE;

	type = ender_item_type_get(i);
	switch (type)
	{
		case ENDER_ITEM_TYPE_BASIC:
		*sz = ender_value_type_size_get(ender_item_basic_value_type_get(i));
		*al = ender_value_type_alignment_get(ender_item_basic_value_type_get(i));
		ret = EINA_TRUE;
		break;

		case ENDER_ITEM_TYPE_DEF:
		other = ender_item_def_type_get(i);
		ret = _ender_item_field_size_alignment_get(other, sz, al);
		ender_item_unref(other);
		break;

		default:
		CRI("Unsupported attr type '%d'", type);
		break;
	}
	return ret;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_struct_new(void)
{
	Ender_Item *i;

	i = ENESIM_OBJECT_INSTANCE_NEW(ender_item_struct);
	return i;
}

void ender_item_struct_field_add(Ender_Item *i, Ender_Item *p)
{
	Ender_Item_Struct *thiz;
	Ender_Item_Type type;
	Ender_Item *attr_type;
	size_t size = 0;
	ssize_t align = 0;

	type = ender_item_type_get(p);
	if (type != ENDER_ITEM_TYPE_ATTR)
	{
		ender_item_unref(p);
		return;
	}
		
	thiz = ENDER_ITEM_STRUCT(i);
	attr_type = ender_item_attr_type_get(p);
	if (!_ender_item_field_size_alignment_get(attr_type, &size, &align))
	{
		CRI("Can not add field '%s' on' %s', wrong things might happen",
			ender_item_name_get(p), ender_item_name_get(i));
	}

	/* add the padding */
	thiz->size = (thiz->size + align - 1) & ~(align - 1);
	ender_item_attr_offset_set(p, thiz->size);
	/* add the size */
	thiz->size += size;

	thiz->fields = eina_list_append(thiz->fields, p);
	ender_item_parent_set(p, i);
}

Eina_Bool ender_item_struct_field_value_get(void *o, Ender_Item *field,
		Ender_Value *v, Eina_Error *err)
{
	Ender_Item *type;
	Eina_Bool ret = EINA_FALSE;
	char *oa;

	oa = ((char *)o) + ender_item_attr_offset_get(field);
	/* when getting a value, it is always transfered with a ref, i.e full */
	type = ender_item_attr_type_get(field);
	switch (ender_item_type_get(type))
	{
		case ENDER_ITEM_TYPE_BASIC:
		{
			switch (ender_item_basic_value_type_get(type))
			{
				case ENDER_VALUE_TYPE_BOOL:
				v->b = *((Eina_Bool *)oa);
				break;

				case ENDER_VALUE_TYPE_UINT8:
				v->u8 = *((uint8_t *)oa);
				break;

				case ENDER_VALUE_TYPE_INT8:
				v->i8 = *((int8_t *)oa);
				break;

				case ENDER_VALUE_TYPE_UINT32:
				v->u32 = *((uint32_t *)oa);
				break;

				case ENDER_VALUE_TYPE_INT32:
				v->i32 = *((int32_t *)oa);
				break;

				case ENDER_VALUE_TYPE_UINT64:
				v->u64 = *((uint64_t *)oa);
				break;

				case ENDER_VALUE_TYPE_INT64:
				v->i64 = *((int64_t *)oa);
				break;

				case ENDER_VALUE_TYPE_DOUBLE:
				v->d = *((double *)oa);
				break;

				case ENDER_VALUE_TYPE_STRING:
				v->ptr = *((char **)oa);
				break;

				case ENDER_VALUE_TYPE_POINTER:
				v->ptr = *((void **)oa);
				break;
			}
			ret = EINA_TRUE;
		}
		break;

		default:
		ERR("Unsupported type");
		break;
	}
	ender_item_unref(type);
	return ret;
}

Eina_Bool ender_item_struct_field_value_set(void *o, Ender_Item *field,
		Ender_Value *v, Eina_Error *err)
{
	Ender_Item *type;
	Eina_Bool ret = EINA_FALSE;
	char *oa;

	oa = ((char *)o) + ender_item_attr_offset_get(field);
	/* when getting a value, it is always transfered with a ref, i.e full */
	type = ender_item_attr_type_get(field);
	switch (ender_item_type_get(type))
	{
		case ENDER_ITEM_TYPE_BASIC:
		{
			switch (ender_item_basic_value_type_get(type))
			{
				case ENDER_VALUE_TYPE_BOOL:
				*((Eina_Bool *)oa) = v->b;
				break;

				case ENDER_VALUE_TYPE_UINT8:
				*((uint8_t *)oa) = v->u8;
				break;

				case ENDER_VALUE_TYPE_INT8:
				*((int8_t *)oa) = v->i8;
				break;

				case ENDER_VALUE_TYPE_UINT32:
				*((uint32_t *)oa) = v->u32;
				break;

				case ENDER_VALUE_TYPE_INT32:
				*((int32_t *)oa) = v->i32;
				break;

				case ENDER_VALUE_TYPE_UINT64:
				*((uint64_t *)oa) = v->u64;
				break;

				case ENDER_VALUE_TYPE_INT64:
				*((int64_t *)oa) = v->i64;
				break;

				case ENDER_VALUE_TYPE_DOUBLE:
				*((double *)oa) = v->d;
				break;

				case ENDER_VALUE_TYPE_STRING:
				*((char **)oa) = v->ptr;
				break;

				case ENDER_VALUE_TYPE_POINTER:
				*((void **)oa) = v->ptr;
				break;
			}
			ret = EINA_TRUE;
		}
		break;

		default:
		ERR("Unsupported type");
		break;
	}
	ender_item_unref(type);
	return ret;
}

void ender_item_struct_function_add(Ender_Item *i, Ender_Item *f)
{
	Ender_Item_Struct *thiz;
	Ender_Item_Type type;

	type = ender_item_type_get(f);
	if (type != ENDER_ITEM_TYPE_FUNCTION)
	{
		ender_item_unref(f);
		return;
	}
		
	thiz = ENDER_ITEM_STRUCT(i);
	thiz->functions = eina_list_append(thiz->functions, f);
	ender_item_parent_set(f, i);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI size_t ender_item_struct_size_get(Ender_Item *i)
{
	Ender_Item_Struct *thiz;

	thiz = ENDER_ITEM_STRUCT(i);
	return thiz->size;
}

EAPI Eina_List * ender_item_struct_fields_get(Ender_Item *i)
{
	Ender_Item_Struct *thiz;
	Ender_Item *f;
	Eina_List *ret = NULL;
	Eina_List *l;

	thiz = ENDER_ITEM_STRUCT(i);
	EINA_LIST_FOREACH(thiz->fields, l, f)
	{
		ret = eina_list_append(ret, ender_item_ref(f));
	}
	return ret;
}

EAPI Eina_List * ender_item_struct_functions_get(Ender_Item *i)
{
	Ender_Item_Struct *thiz;
	Ender_Item *f;
	Eina_List *ret = NULL;
	Eina_List *l;

	thiz = ENDER_ITEM_STRUCT(i);
	EINA_LIST_FOREACH(thiz->functions, l, f)
	{
		ret = eina_list_append(ret, ender_item_ref(f));
	}
	return ret;
}

