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
#include "Ender.h"
#include "ender_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
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
 *                     uint32 / in32 / argb / bool                            *
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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Descriptor_Backend ender_struct_backend = {
	/* .validate 		= */ _ender_struct_validate,
	/* .creator 		= */ _ender_struct_creator,
	/* .destructor 		= */ _ender_struct_destructor,
	/* .property_add 	= */ _ender_struct_property_add,
	/* .size_get 		= */ _ender_struct_size_get,
};

void ender_struct_init(void)
{
	int i;

	/* initialize to some sane values */
	for (i = 0; i < ENDER_PROPERTY_TYPES; i++)
	{
		_setters[i] = _ender_dummy_set;
		_getters[i] = _ender_dummy_get;
	}
	/* setters */
	_setters[ENDER_BOOL] = _ender_int32_set;
	_setters[ENDER_UINT32] = _ender_int32_set;
	_setters[ENDER_INT32] = _ender_int32_set;
	_setters[ENDER_UINT64] = _ender_int64_set;
	_setters[ENDER_INT64] = _ender_int64_set;
	_setters[ENDER_DOUBLE] = _ender_double_set;
	_setters[ENDER_ARGB] = _ender_int32_set;
	_setters[ENDER_COLOR] = _ender_int32_set;
	_setters[ENDER_STRING] = _ender_pointer_set;
	_setters[ENDER_OBJECT] = _ender_object_set;
	_setters[ENDER_ENDER] = _ender_object_set;
	_setters[ENDER_LIST] = _ender_pointer_set;
	_setters[ENDER_STRUCT] = _ender_pointer_set;
	_setters[ENDER_UNION] = _ender_pointer_set;
	/* getters */
	_getters[ENDER_BOOL] = _ender_int32_get;
	_getters[ENDER_UINT32] = _ender_int32_get;
	_getters[ENDER_INT32] = _ender_int32_get;
	_getters[ENDER_UINT64] = _ender_int64_get;
	_getters[ENDER_INT64] = _ender_int64_get;
	_getters[ENDER_DOUBLE] = _ender_double_get;
	_getters[ENDER_ARGB] = _ender_int32_get;
	_getters[ENDER_COLOR] = _ender_int32_get;
	_getters[ENDER_STRING] = _ender_object_get;
	_getters[ENDER_OBJECT] = _ender_object_get;
	_getters[ENDER_ENDER] = _ender_object_get;
	_getters[ENDER_LIST] = _ender_object_get;
	_getters[ENDER_STRUCT] = _ender_pointer_get;
	_getters[ENDER_UNION] = _ender_pointer_get;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

