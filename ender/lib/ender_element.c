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
#define ENDER_MAGIC 0xe743e001
#define ENDER_MAGIC_CHECK(d)						\
	do {								\
		if (!EINA_MAGIC_CHECK(d, ENDER_MAGIC))			\
			EINA_MAGIC_FAIL(d, ENDER_MAGIC);		\
	} while(0)

#define ENDER_VALUE_FROM_DATA(v, cnt, args)				\
	{								\
		v.container = cnt;					\
		switch (cnt->type)					\
		{							\
			case ENDER_INT32:				\
			v.data.i32 = va_arg(args, int32_t);		\
			break;						\
									\
			case ENDER_COLOR:				\
			case ENDER_ARGB:				\
			case ENDER_UINT32:				\
			case ENDER_BOOL:				\
			v.data.u32 = va_arg(args, uint32_t);		\
			break;						\
									\
			case ENDER_UINT64:				\
			case ENDER_INT64:				\
			v.data.u64 = va_arg(args, uint64_t);		\
			break;						\
									\
			case ENDER_DOUBLE:				\
			v.data.d = va_arg(args, double);		\
			break;						\
									\
			case ENDER_STRUCT:				\
			case ENDER_UNION:				\
			case ENDER_STRING:				\
			case ENDER_OBJECT:				\
			case ENDER_ENDER:				\
			case ENDER_LIST:				\
			case ENDER_VALUE:				\
			v.data.ptr = va_arg(args, void *);		\
			break;						\
									\
			default:					\
			ERR("Unsupported data type %d", cnt->type);	\
			break;						\
		}							\
	}								\

#define ENDER_VALUE_TO_DATA(v, cnt, args)				\
	{								\
		switch (cnt->type)					\
		{							\
			case ENDER_COLOR:				\
			case ENDER_ARGB:				\
			case ENDER_UINT32:				\
			case ENDER_INT32:				\
			case ENDER_BOOL:				\
			*(va_arg(var_args, uint32_t*)) = v.data.u32;	\
			break;						\
									\
			case ENDER_UINT64:				\
			case ENDER_INT64:				\
			*(va_arg(var_args, uint64_t*)) = v.data.u64;	\
			break;						\
									\
			case ENDER_DOUBLE:				\
			*(va_arg(var_args, double*)) = v.data.d;	\
			break;						\
									\
			case ENDER_STRUCT:				\
			case ENDER_UNION:				\
			case ENDER_STRING:				\
			case ENDER_OBJECT:				\
			case ENDER_ENDER:				\
			case ENDER_LIST:				\
			case ENDER_VALUE:				\
			*(va_arg(var_args, void**)) = v.data.ptr;	\
			break;						\
									\
			default:					\
			ERR("Unsupported data type %d", cnt->type);	\
			break;						\
		}							\
	}								\

#define ENDER_VALUE_FETCH(v, cnt, fetch)				\
	{								\
		switch (cnt->type)					\
		{							\
			case ENDER_COLOR:				\
			case ENDER_ARGB:				\
			case ENDER_UINT32:				\
			case ENDER_INT32:				\
			case ENDER_BOOL:				\
			*((uint32_t*)fetch) = v.data.u32;		\
			break;						\
									\
			case ENDER_UINT64:				\
			case ENDER_INT64:				\
			*((uint64_t*)fetch) = v.data.u64;		\
			break;						\
									\
			case ENDER_DOUBLE:				\
			*((double*)fetch) = v.data.d;			\
			break;						\
									\
			case ENDER_STRUCT:				\
			case ENDER_UNION:				\
			case ENDER_STRING:				\
			case ENDER_OBJECT:				\
			case ENDER_ENDER:				\
			case ENDER_LIST:				\
			case ENDER_VALUE:				\
			*((void**)fetch) = v.data.ptr;			\
			break;						\
									\
			default:					\
			ERR("Unsupported data type %d", cnt->type);	\
			break;						\
		}							\
	}								\


typedef struct _Ender_Listener_Container
{
	char *name;
	Eina_List *listeners;
} Ender_Listener_Container;

/* TODO rename this */
struct _Ender_Listener
{
	Ender_Event_Callback callback;
	Ender_Listener_Container *container;
	void *data;
};

typedef struct _Ender_Element_Property_List_Data
{
	Ender_Property_List_Callback cb;
	void *data;
} Ender_Element_Property_List_Data;

typedef struct _Ender_Element_Property
{
	Ender_Element_Getter get;
	Ender_Element_Setter set;
	Ender_Element_Add add;
	Ender_Element_Remove remove;
	Ender_Element_Clear clear;
	Ender_Element_Is_Set is_set;
	void *data;
} Ender_Element_Property;

struct _Ender_Element
{
	EINA_MAGIC
	Ender_Descriptor *descriptor;
	void *object;
	Eina_Hash *listeners;
	Eina_Hash *properties;
	Eina_Hash *data;
	int ref;
};

static void _ender_element_delete(Ender_Element *e)
{
	Ender_Descriptor *desc = e->descriptor;

	/* call the free callback */
	ender_event_dispatch(e, "Free", NULL);
	ender_descriptor_native_destroy(desc, e->object);
	/* free every private data */
	/* TODO the listeners */
	eina_hash_free(e->properties);
	eina_hash_free(e->data);
	free(e);
}

static Ender_Property * _ender_element_property_get_simple(Ender_Element *e,
		const char *name)
{
	Ender_Property *prop;

	/* first check dynamic properties */
	prop = eina_hash_find(e->properties, name);
	if (prop) return prop;
	/* now the static properties */
	prop = ender_descriptor_property_get(e->descriptor, name);
	return prop;
}

static Ender_Property * _ender_element_property_parent_get(Ender_Element *e)
{
	Ender_Property *prop;
	Ender_Value_Type type;

	prop = _ender_element_property_get_simple(e, "parent");
	if (!prop) return prop;

	type = ender_property_type_get(prop);
	if (type != ENDER_ENDER)
		return prop;

	return prop;
}

static void _property_is_relative_cb(Ender_Property *prop, void *data)
{
	Ender_Element_Property_List_Data *new_data = data;

	if (ender_property_is_relative(prop))
		new_data->cb(prop, new_data->data);
}

static void _ender_element_property_free(void *data)
{
	Ender_Property *p = data;

}
/*----------------------------------------------------------------------------*
 *                        The property interface                              *
 *----------------------------------------------------------------------------*/
static void _property_set(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Element_Property *eprop = data;

	eprop->set(e, p, v, eprop->data);
}

static void _property_get(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Element_Property *eprop = data;

	eprop->get(e, p, v, eprop->data);
}

static void _property_add(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Element_Property *eprop = data;

	eprop->add(e, p, v, eprop->data);
}

static void _property_remove(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Element_Property *eprop = data;

	eprop->remove(e, p, v, eprop->data);
}

static void _property_clear(Ender_Property *p, Ender_Element *e, void *data)
{
	Ender_Element_Property *eprop = data;

	eprop->clear(e, p, eprop->data);
}

static Eina_Bool _property_is_set(Ender_Property *p, Ender_Element *e, void *data)
{
	Ender_Element_Property *eprop = data;

	return eprop->is_set(e, p, eprop->data);
}

static void _property_free(void *data)
{
	Ender_Element_Property *eprop = data;
	/* TODO free the user provided data */
	free(eprop);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Element * ender_element_new_from_data(Ender_Descriptor *desc, void *data)
{
	Ender_Element *thiz;

	thiz = calloc(1, sizeof(Ender_Element));
	EINA_MAGIC_SET(thiz, ENDER_MAGIC);
	thiz->object = data;
	thiz->descriptor = desc;
	thiz->listeners = eina_hash_string_superfast_new(NULL);
	thiz->properties = eina_hash_string_superfast_new((Eina_Free_Cb)ender_property_free);
	thiz->data = eina_hash_string_superfast_new(NULL);
	thiz->ref = 1;

	return thiz;
}

Ender_Element * ender_element_new(Ender_Descriptor *desc)
{
	void *object;

	if (!desc)
	{
		ERR("No such descriptor for name");
		return NULL;
	}

	DBG("Creating new ender '%s'", desc->name);
	object = ender_descriptor_native_create(desc);
	if (!object)
	{
		ERR("For some reason the creator for '%s' failed", desc->name);
		return NULL;
	}
	DBG("Element '%s' created correctly", desc->name);
	return ender_element_new_from_data(desc, object);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_element_new_with_namespace(const char *name, const char *ns_name, int version)
{
	Ender_Namespace *ns;
	Ender_Descriptor *desc;

	ns = ender_namespace_find(ns_name, version);
	if (!ns) return NULL;
	return ender_namespace_element_new(ns, name);
}

/**
 * Ref an ender
 */
EAPI Ender_Element * ender_element_ref(Ender_Element *e)
{
	ENDER_MAGIC_CHECK(e);
	e->ref++;
	return e;
}

/**
 * Unref an ender
 */
EAPI Ender_Element * ender_element_unref(Ender_Element *e)
{
	ENDER_MAGIC_CHECK(e);
	e->ref--;
	if (!e->ref)
	{
		_ender_element_delete(e);
		return NULL;
	}
	return e;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * ender_element_data_set(Ender_Element *e, const char *key, void *data)
{
	void *old_data;

	if (!key) return NULL;

	ENDER_MAGIC_CHECK(e);
	old_data = eina_hash_find(e->data, key);
	eina_hash_add(e->data, key, data);

	return old_data;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * ender_element_data_get(Ender_Element *e, const char *key)
{
	if (!key) return NULL;

	ENDER_MAGIC_CHECK(e);
	return eina_hash_find(e->data, key);
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Descriptor * ender_element_descriptor_get(Ender_Element *e)
{
	return e->descriptor;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * ender_element_name_get(Ender_Element *e)
{
	ENDER_MAGIC_CHECK(e);
	return ender_descriptor_name_get(e->descriptor);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_add_valist(Ender_Element *e, const char *name, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (name)
	{
		Ender_Property *prop;
		Ender_Container *ec;
		Ender_Value v;

		prop = ender_descriptor_property_get(e->descriptor, name);
		if (!prop) break;
		ec = ender_property_container_get(prop);
		if (ender_container_type_get(ec) != ENDER_LIST) break;
		ec = ender_container_compound_get(ec, 0, NULL);
		if (!ec)
		{
			ERR("Property '%s' does not have a sub-container",
					ender_property_name_get(prop));
			break;
		}

		ENDER_VALUE_FROM_DATA(v, ec, var_args);
		ender_property_element_value_add(prop, e, &v);
		name = va_arg(var_args, char *);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_add(Ender_Element *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, name);
	ender_element_value_add_valist(e, name, ap);
	va_end(ap);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_add_simple(Ender_Element *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_element_property_get(e, name);
	if (!prop) return;
	ender_property_element_value_add(prop, e, value);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_remove_valist(Ender_Element *e, const char *name, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (name)
	{
		Ender_Property *prop;
		Ender_Container *ec;
		Ender_Value v;

		prop = ender_descriptor_property_get(e->descriptor, name);
		if (!prop) break;
		ec = ender_property_container_get(prop);
		if (ender_container_type_get(ec) != ENDER_LIST) break;
		ec = ender_container_compound_get(ec, 0, NULL);
		if (!ec)
		{
			ERR("Property '%s' does not have a sub-container",
					ender_property_name_get(prop));
			break;
		}

		ENDER_VALUE_FROM_DATA(v, ec, var_args);
		ender_property_element_value_remove(prop, e, &v);
		name = va_arg(var_args, char *);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_remove(Ender_Element *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, name);
	ender_element_value_remove_valist(e, name, ap);
	va_end(ap);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_remove_simple(Ender_Element *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_element_property_get(e, name);
	if (!prop) return;
	ender_property_element_value_remove(prop, e, value);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_clear(Ender_Element *e, const char *name)
{
	Ender_Property *prop;
	Ender_Container *ec;

	ENDER_MAGIC_CHECK(e);
	prop = ender_descriptor_property_get(e->descriptor, name);
	if (!prop) return;
	ec = ender_property_container_get(prop);
	if (ender_container_type_get(ec) != ENDER_LIST) return;
	ender_property_element_value_clear(prop, e);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_element_value_is_set(Ender_Element *e, const char *name)
{
	Ender_Property *prop;
	Ender_Container *ec;

	ENDER_MAGIC_CHECK(e);
	prop = ender_descriptor_property_get(e->descriptor, name);
	if (!prop) return EINA_FALSE;
	return ender_property_element_value_is_set(prop, e);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_get_valist(Ender_Element *e, const char *name, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);
	while (name)
	{
		Ender_Value v;
		Ender_Property *prop;
		Ender_Container *ec;

		prop = ender_element_property_get(e, name);
		if (!prop) return;

		ec = ender_property_container_get(prop);
		v.container = ec;
		ender_property_element_value_get(prop, e, &v);
		ENDER_VALUE_TO_DATA(v, ec, var_args);
		name = va_arg(var_args, char *);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_get(Ender_Element *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);
	va_start(ap, name);
	ender_element_value_get_valist(e, name, ap);
	va_end(ap);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_get_simple(Ender_Element *e, const char *name, Ender_Value **value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_descriptor_property_get(e->descriptor, name);
	if (!prop) return;

	ender_element_property_value_get_simple(e, prop, value);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_set_valist(Ender_Element *e, const char *name, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (name)
	{
		Ender_Property *prop;
		Ender_Container *ec;
		Ender_Value v;

		prop = ender_element_property_get(e, name);
		if (!prop) return;
		ec = ender_property_container_get(prop);

		ENDER_VALUE_FROM_DATA(v, ec, var_args);
		ender_property_element_value_set(prop, e, &v);
		name = va_arg(var_args, char *);
	} 
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_set(Ender_Element *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, name);
	ender_element_value_set_valist(e, name, ap);
	va_end(ap);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_value_set_simple(Ender_Element *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_element_property_get(e, name);
	if (!prop) return;
	ender_property_element_value_set(prop, e, value);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Property * ender_element_property_add(Ender_Element *e, const char *name,
		Ender_Container *ec,
		Ender_Element_Getter get,
		Ender_Element_Setter set,
		Ender_Element_Add add,
		Ender_Element_Remove remove,
		Ender_Element_Clear clear,
		Ender_Element_Is_Set is_set,
		Eina_Bool relative, void *data)
{
	Ender_Property *prop;
	Ender_Element_Property *eprop;

	ENDER_MAGIC_CHECK(e);
	prop = ender_element_property_get(e, name);
	if (prop)
	{
		WRN("Property %s already found. Not adding it", name);
		return NULL;
	}
	eprop = calloc(1, sizeof(Ender_Element_Property));
	eprop->get = get;
	eprop->set = set;
	eprop->add = add;
	eprop->remove = remove;
	eprop->clear = clear;
	eprop->is_set = is_set;
	eprop->data = data;

	prop = ender_property_new(name, ec,
			_property_get,
			_property_set,
			_property_add,
			_property_remove,
			_property_clear,
			_property_is_set,
			relative,
			_property_free, eprop);
	eina_hash_add(e->properties, name, prop);
	DBG("Property %s added", name);

	return prop;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Property * ender_element_property_get(Ender_Element *e, const char *name)
{
	Ender_Property *prop;
	Ender_Element *parent;

	ENDER_MAGIC_CHECK(e);

	/* the simplest case */
	prop = _ender_element_property_get_simple(e, name);
	if (prop) return prop;

	/* now check relative properties */
	parent = ender_element_parent_get(e);
	if (!parent)
	{
		WRN("Property %s not found and it has no parent", name);
		return NULL;
	}

	/* get the property from the parent */
	prop = ender_element_property_get(parent, name);
	if (!prop)
	{
		WRN("Parent does not have the property %s", name);
		return NULL;
	}
	if (!ender_property_is_relative(prop))
	{
		WRN("Parent do have the property %s but is not marked as relative", name);
		return NULL;
	}

	return prop;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_list(Ender_Element *e, Ender_Property_List_Callback cb, void *data)
{
	Ender_Element_Property_List_Data new_data;
	Ender_Element *parent;
	Ender_Descriptor *desc;

	ENDER_MAGIC_CHECK(e);

	/* we first list all the local properties */
	desc = e->descriptor;
	do
	{
		ender_descriptor_property_list(desc, cb, data);
	}
	while ((desc = ender_descriptor_parent(desc)));
	/* now let's list the relative properties from the parent */
	parent = ender_element_parent_get(e);
	if (!parent) return;

	new_data.data = data;
	new_data.cb = cb;
	ender_element_property_list(parent, _property_is_relative_cb, &new_data);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_set_valist(Ender_Element *e, Ender_Property *prop, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (prop)
	{
		Ender_Value v;
		Ender_Container *ec;

		ec = ender_property_container_get(prop);
		ENDER_VALUE_FROM_DATA(v, ec, var_args);
		ender_property_element_value_set(prop, e, &v);
		prop = va_arg(var_args, Ender_Property *);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_set(Ender_Element *e, Ender_Property *prop, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, prop);
	ender_element_property_value_set_valist(e, prop, ap);
	va_end(ap);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_set_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value)
{
	ENDER_MAGIC_CHECK(e);

	ender_property_element_value_set(prop, e, value);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_add_valist(Ender_Element *e, Ender_Property *prop, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (prop)
	{
		Ender_Value v;
		Ender_Container *ec;

		ec = ender_property_container_get(prop);
		if (ender_container_type_get(ec) != ENDER_LIST) break;
		ec = ender_container_compound_get(ec, 0, NULL);
		if (!ec)
		{
			ERR("Property '%s' does not have a sub-container",
					ender_property_name_get(prop));
			break;
		}

		ENDER_VALUE_FROM_DATA(v, ec, var_args);
		ender_property_element_value_add(prop, e, &v);
		prop = va_arg(var_args, Ender_Property *);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_add(Ender_Element *e, Ender_Property *prop, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, prop);
	ender_element_property_value_add_valist(e, prop, ap);
	va_end(ap);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_add_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value)
{
	ENDER_MAGIC_CHECK(e);

	ender_property_element_value_add(prop, e, value);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_get_valist(Ender_Element *e, Ender_Property *prop, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (prop)
	{
		Ender_Value v;
		Ender_Value_Type type;
		Ender_Container *ec;

		ec = ender_property_container_get(prop);
		type = ender_container_type_get(ec);
		v.container = ec;
		v.data.ptr = NULL;

		/* for structs and unions we ned the pointer from the passed in value */
		if (type == ENDER_STRUCT || type == ENDER_UNION)
		{
			ENDER_VALUE_FROM_DATA(v, ec, var_args);
			ender_property_element_value_get(prop, e, &v);
		}
		else
		{
			ender_property_element_value_get(prop, e, &v);
			ENDER_VALUE_TO_DATA(v, ec, var_args);
		}
		prop = va_arg(var_args, Ender_Property *);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_remove_valist(Ender_Element *e, Ender_Property *prop, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (prop)
	{
		Ender_Value v;
		Ender_Container *ec;

		ec = ender_property_container_get(prop);
		if (ender_container_type_get(ec) != ENDER_LIST) break;
		ec = ender_container_compound_get(ec, 0, NULL);
		if (!ec)
		{
			ERR("Property '%s' does not have a sub-container",
					ender_property_name_get(prop));
			break;
		}

		ENDER_VALUE_FROM_DATA(v, ec, var_args);
		ender_property_element_value_remove(prop, e, &v);
		prop = va_arg(var_args, Ender_Property *);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_remove(Ender_Element *e, Ender_Property *prop, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, prop);
	ender_element_property_value_remove_valist(e, prop, ap);
	va_end(ap);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_remove_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value)
{
	ENDER_MAGIC_CHECK(e);

	ender_property_element_value_remove(prop, e, value);
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_get(Ender_Element *e, Ender_Property *prop, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);
	if (!prop) return;

	va_start(ap, prop);
	ender_element_property_value_get_valist(e, prop, ap);
	va_end(ap);
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_property_value_get_simple(Ender_Element *e, Ender_Property *prop, Ender_Value **value)
{
	ENDER_MAGIC_CHECK(e);
	if (!prop) return;
	if (!value) return;

	if (!*value)
	{
		Ender_Container *ec;

		ec = ender_property_container_get(prop);
		*value = ender_value_new_container_static_from(ec);
	}

	ender_property_element_value_get(prop, e, *value);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_element_property_value_is_set(Ender_Element *e, Ender_Property *prop)
{
	ENDER_MAGIC_CHECK(e);
	if (!prop) return EINA_FALSE;

	return ender_property_element_value_is_set(prop, e);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_element_call_valist(Ender_Element *e, const char *name, void *ret, va_list va_args)
{
	Ender_Function *f;
	Ender_Descriptor *d;

	ENDER_MAGIC_CHECK(e);
	if (!name) return EINA_FALSE;

	/* get the function associated */
	d = ender_element_descriptor_get(e);
	f = ender_descriptor_function_get(d, name);
	if (!f) return EINA_FALSE;

	return ender_element_function_call_valist(e, f, ret, va_args);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_element_call(Ender_Element *e, const char *name, void *arg_ret, ...)
{
	Eina_Bool ret;
	va_list va_args;

	ENDER_MAGIC_CHECK(e);
	if (!name) return EINA_FALSE;

	va_start(va_args, arg_ret);
	ret = ender_element_call_valist(e, name, arg_ret, va_args);
	va_end(va_args);
	return ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_element_function_call_valist(Ender_Element *e, Ender_Function *f, void *ret, va_list va_args)
{
	Eina_List *largs = NULL;
	Eina_List *lvalues = NULL;
	Eina_List *l;
	Ender_Container *c;
	Ender_Value vret = { 0 };

	ENDER_MAGIC_CHECK(e);
	if (!f) return EINA_FALSE;

	largs = (Eina_List *) ender_function_args_get(f);
	/* add a value for the ret */
	c = ender_function_ret_get(f);
	vret.container = c;
	EINA_LIST_FOREACH (largs, l, c)
	{
		Ender_Value *value;

		value = alloca(sizeof(Ender_Value));
		ENDER_VALUE_FROM_DATA((*value), c, va_args);
		lvalues = eina_list_append(lvalues, value);
	}
	/* call the real function */
	if (!ender_function_call(f, e->object, &vret, lvalues))
		return EINA_FALSE;
	if (ret)
	{
		ENDER_VALUE_FETCH(vret, vret.container, ret);
	}
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_element_function_call_simple(Ender_Element *e,
		Ender_Function *f, Ender_Value *ret, Eina_List *args)
{
	ENDER_MAGIC_CHECK(e);
	if (!f) return EINA_FALSE;

	return ender_function_call(f, e->object, ret, args);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_element_function_call(Ender_Element *e, Ender_Function *f, void *arg_ret, ...)
{
	Eina_Bool ret;
	va_list va_args;

	ENDER_MAGIC_CHECK(e);
	if (!f) return EINA_FALSE;

	va_start(va_args, arg_ret);
	ret = ender_element_function_call_valist(e, f, arg_ret, va_args);
	va_end(va_args);
	return ret;
}


/**
 * Get the native object associated with an ender
 * @param e
 * @return
 */
EAPI void * ender_element_object_get(Ender_Element *e)
{
	ENDER_MAGIC_CHECK(e);
	return e->object;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_element_parent_get(Ender_Element *e)
{
	Ender_Property *prop;
	Ender_Element *parent;
	Ender_Container *container;

	ENDER_MAGIC_CHECK(e);

	/* FIXME right now we only support parents that are enders
	 * but we also need parents that are objects ...
	 */
	prop = _ender_element_property_parent_get(e);
	if (!prop) return NULL;
	container = ender_property_container_get(prop);
	if (ender_container_type_get(container) != ENDER_ENDER)
		return NULL;
	ender_element_property_value_get(e, prop, &parent, NULL);

	return parent;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Listener * ender_event_listener_add(Ender_Element *e, const char *event_name,
		Ender_Event_Callback cb, void *data)
{
	Ender_Listener *listener;
	Ender_Listener_Container *container;

	ENDER_MAGIC_CHECK(e);

	container = eina_hash_find(e->listeners, event_name);
	if (!container)
	{
		container = calloc(1, sizeof(Ender_Listener_Container));
		container->name = strdup(event_name);
		eina_hash_add(e->listeners, event_name, container);
	}
	listener = calloc(1, sizeof(Ender_Listener));
	listener->callback = cb;
	listener->data = data;
	listener->container = container;

	container->listeners = eina_list_append(container->listeners, listener);
	return listener;
}

EAPI void ender_event_listener_remove(Ender_Listener *l)
{
	Ender_Listener_Container *container;
	if (!l) return;

	container = l->container;
	container->listeners = eina_list_remove(container->listeners, l);
	free(l);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_event_listener_remove_full(Ender_Element *e, const char *name,
		Ender_Event_Callback cb, void *data)
{
	Ender_Listener_Container *container;
	Ender_Listener *listener;
	Eina_List *l;

	ENDER_MAGIC_CHECK(e);
	container = eina_hash_find(e->listeners, name);
	if (!container) return;

	EINA_LIST_FOREACH(container->listeners, l, listener)
	{
		if (listener->callback == cb && listener->data == data)
		{
			ender_event_listener_remove(listener);
			break;
		}
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_event_dispatch(Ender_Element *e, const char *event_name, void *event_data)
{
	Ender_Listener *listener;
	Ender_Listener_Container *container;
	Eina_List *l;

	ENDER_MAGIC_CHECK(e);
	container = eina_hash_find(e->listeners, event_name);
	if (!container) return;

	EINA_LIST_FOREACH(container->listeners, l, listener)
	{
		listener->callback(e, event_name, event_data, listener->data);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * ender_element_marshal(Ender_Element *e, unsigned int *len)
{
	return ender_serializer_element_marshal(e, len);
}
