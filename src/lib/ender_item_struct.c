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

#include "ender_main_private.h"
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
	size_t size;
} Ender_Item_Struct;

typedef struct _Ender_Item_Struct_Class
{
	Ender_Item_Class base;
} Ender_Item_Struct_Class;

#if 0
typedef struct _Ender_Struct_Property
{
	size_t offset;
	Ender_Descriptor_Property dprop;
} Ender_Struct_Property;

typedef void (*Ender_Value_Accessor)(Ender_Value *v, void *o);
static Ender_Value_Accessor _setters[ENDER_PROPERTY_TYPES];
static Ender_Value_Accessor _getters[ENDER_PROPERTY_TYPES];

static size_t _ender_struct_size_get(Ender_Descriptor *d);

/* just pick the last property */
static void _ender_struct_property_get_last(Ender_Property *prop, void *data)
{
	Ender_Property **ret = data;
	*ret = prop;
}

/* get the final size */
static void _ender_struct_property_get_size(Ender_Property *prop, void *data)
{
	Ender_Container *c;
	size_t *size = data;

	c = ender_property_container_get(prop);
	*size += ender_container_size_get(c);
	ender_container_unref(c);
}
/*----------------------------------------------------------------------------*
 *                                 int32                                      *
 *----------------------------------------------------------------------------*/
static void _ender_int32_get(Ender_Value *v, void *o)
{
	v->data.i32 = *(int32_t *)o;
}

static void _ender_int32_set(Ender_Value *v, void *o)
{
	*(int32_t *)o = v->data.i32;
}
/*----------------------------------------------------------------------------*
 *                        uint32 / argb / bool                                *
 *----------------------------------------------------------------------------*/
static void _ender_uint32_get(Ender_Value *v, void *o)
{
	v->data.u32 = *(uint32_t *)o;
}

static void _ender_uint32_set(Ender_Value *v, void *o)
{
	*(uint32_t *)o = v->data.u32;
}
/*----------------------------------------------------------------------------*
 *                              uint64 / in64                                 *
 *----------------------------------------------------------------------------*/
static void _ender_int64_get(Ender_Value *v, void *o)
{
	v->data.i64 = *(int64_t *)o;
}

static void _ender_int64_set(Ender_Value *v, void *o)
{
	*(int64_t *)o = v->data.i64;
}
/*----------------------------------------------------------------------------*
 *                                   double                                   *
 *----------------------------------------------------------------------------*/
static void _ender_double_get(Ender_Value *v, void *o)
{
	v->data.d = *(double *)o;
}

static void _ender_double_set(Ender_Value *v, void *o)
{
	*(double *)o = v->data.d;
}
/*----------------------------------------------------------------------------*
 *                                  pointer                                   *
 *----------------------------------------------------------------------------*/
/* used for known pointers: struct, unions, etc */
static void _ender_pointer_get(Ender_Value *v, void *o)
{
	v->data.ptr = *(void **)o;
}

/* used for string, struct, union, object */
static void _ender_pointer_set(Ender_Value *v, void *o)
{
	*(void **)o = v->data.ptr;
}
/*----------------------------------------------------------------------------*
 *                                 objects                                    *
 *----------------------------------------------------------------------------*/
/* valid for every object: ender, object, etc ... */
static void _ender_object_get(Ender_Value *v, void *o)
{
	v->data.ptr = *(void **)o;
}
static void _ender_object_set(Ender_Value *v, void *o)
{
	*(void **)o = v->data.ptr;
}
/*----------------------------------------------------------------------------*
 *                                   dummy                                    *
 *----------------------------------------------------------------------------*/
static void _ender_dummy_set(Ender_Value *v, void *o)
{

}

static void _ender_dummy_get(Ender_Value *v, void *o)
{

}
/*----------------------------------------------------------------------------*
 *                        The property interface                              *
 *----------------------------------------------------------------------------*/
static void _property_set(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Struct_Property *sprop = data;
	void *object;

	object = ender_element_object_get(e);
	_setters[v->container->type](v, (char *)object + sprop->offset);
}

static void _property_get(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Struct_Property *sprop = data;
	void *object;

	object = ender_element_object_get(e);
	_getters[v->container->type](v, (char *)object + sprop->offset);
}

static void _property_ext_set(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Struct_Property *sprop = data;
	ender_descriptor_object_property_set(p, e, v, &sprop->dprop);
}

static void _property_ext_get(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Struct_Property *sprop = data;
	ender_descriptor_object_property_get(p, e, v, &sprop->dprop);
}

static void _property_ext_clear(Ender_Property *p, Ender_Element *e, void *data)
{
	Ender_Struct_Property *sprop = data;
	ender_descriptor_object_property_clear(p, e, &sprop->dprop);
}

static Eina_Bool _property_ext_is_set(Ender_Property *p, Ender_Element *e, void *data)
{
	Ender_Struct_Property *sprop = data;
	void *object;

	return ender_descriptor_object_property_is_set(p, e, &sprop->dprop);
}

static void _property_free(void *data)
{
	Ender_Struct_Property *sprop = data;
	free(sprop);
}

/*----------------------------------------------------------------------------*
 *                       The descriptor interface                             *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_struct_validate(const char *name, Ender_Namespace *ns,
		Ender_Creator creator,
		Ender_Destructor destructor,
		Ender_Descriptor *parent, Ender_Descriptor_Type type)
{
	if (parent)
	{
		ERR("Struct '%s' can not have a parent yet", name);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static void * _ender_struct_creator(Ender_Descriptor *d)
{
	void *ret;

	if (d->create)
	{
		ret = d->create();
	}
	else
	{
		size_t size = 0;

		/* iterate over the list of properties and get the final size */
		ender_descriptor_property_list(d, _ender_struct_property_get_size, &size);
		if (!size) return NULL;
		ret = calloc(1, size);
	}
	return ret;
}

static void _ender_struct_destructor(Ender_Descriptor *d, void *n)
{
	if (d->destroy)
	{
		d->destroy(n);
	}
	else
	{
		free(n);
	}
}

static Ender_Property * _ender_struct_property_add(Ender_Descriptor *d,
		const char *name, Ender_Container *ec, Ender_Getter get,
		Ender_Setter set, Ender_Add add, Ender_Remove remove,
		Ender_Clear clear, Ender_Is_Set is_set, Eina_Bool relative,
		int offset)
{
	Ender_Struct_Property *sprop;
	Ender_Property *prop;

	if (relative)
	{
		ERR("Structs do not support relative properties");
		return NULL;
	}

	sprop = calloc(1, sizeof(Ender_Struct_Property));
	sprop->dprop.get = get;
	sprop->dprop.set = set;
	sprop->dprop.add = add;
	sprop->dprop.remove = remove;
	sprop->dprop.clear = clear;
	sprop->dprop.clear = clear;
	/* in case the property does not have an offset, use the last
	 * known size for it
	 */
	if (offset < 0)
		offset = _ender_struct_size_get(d);
	sprop->offset = offset;

	prop = ender_property_new(name, ec,
			get ? _property_ext_get : _property_get,
			set ? _property_ext_set : _property_set,
			add ? _property_ext_set : NULL,
			remove ? _property_ext_set : NULL,
			clear ? _property_ext_clear : NULL,
			is_set ? _property_ext_is_set : NULL,
			relative,
			_property_free, sprop);
	return prop;
}

static size_t _ender_struct_size_get(Ender_Descriptor *d)
{
	Ender_Property *last = NULL;
	size_t size = 0;

	ender_descriptor_property_list(d, _ender_struct_property_get_last, &last);
	/* we have a last property, set the correct offset */
	if (last)
	{
		Ender_Struct_Property *slast;
		Ender_Container *last_container;
		size_t last_size;

		last_container = ender_property_container_get(last);
		slast = ender_property_data_get(last);
		last_size = ender_container_size_get(last_container);
		ender_container_unref(last_container);
		size = slast->offset + last_size;
	}
	return size;
}
#endif
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
	type = ender_item_type_get(attr_type);
	switch (type)
	{
		case ENDER_ITEM_TYPE_BASIC:
		size = ender_value_type_size_get(ender_item_basic_value_type_get(attr_type));
		align = ender_value_type_alignment_get(ender_item_basic_value_type_get(attr_type));
		break;

		default:
		CRI("Unsupported attr type '%d'", type);
		break;
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
