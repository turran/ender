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
/**
 * @todo
 * - Add dynamic properties, we must export the property_add function for
 * descriptors given that those *must* be fixed always and impossible to remove
 * but properties that are added through a element_property_add/remove must
 * be available
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_MAGIC 0xe743e001
#define ENDER_MAGIC_CHECK(d)						\
	do {								\
		if (!EINA_MAGIC_CHECK(d, ENDER_MAGIC))			\
			EINA_MAGIC_FAIL(d, ENDER_MAGIC);		\
	} while(0)

#define ENDER_VALUE_COLLECT(v, cnt, args)				\
	{								\
		v.container = cnt;					\
		switch (cnt->type)					\
		{							\
			case ENDER_COLOR:				\
			case ENDER_ARGB:				\
			case ENDER_UINT32:				\
			case ENDER_INT32:				\
			case ENDER_BOOL:				\
			v.data.u32 = va_arg(var_args, uint32_t);	\
			break;						\
									\
			case ENDER_DOUBLE:				\
			v.data.d = va_arg(var_args, double);		\
			break;						\
									\
			case ENDER_STRUCT:				\
			case ENDER_STRING:				\
			case ENDER_SURFACE:				\
			case ENDER_MATRIX:				\
			case ENDER_RENDERER:				\
			case ENDER_ENDER:				\
			case ENDER_LIST:				\
			case ENDER_VALUE:				\
			v.data.ptr = va_arg(var_args, void *);		\
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
			case ENDER_DOUBLE:				\
			*(va_arg(var_args, double*)) = v.data.d;	\
			break;						\
									\
			case ENDER_STRUCT:				\
			case ENDER_STRING:				\
			case ENDER_SURFACE:				\
			case ENDER_RENDERER:				\
			case ENDER_ENDER:				\
			case ENDER_LIST:				\
			case ENDER_VALUE:				\
			*(va_arg(var_args, void**)) = v.data.ptr;	\
			break;						\
									\
			case ENDER_MATRIX:				\
			default:					\
			ERR("Unsupported data type %d", cnt->type);	\
			break;						\
		}							\
	}								\

static Eina_List *_new_callbacks = NULL;

/* TODO rename this */
typedef struct _Ender_Listener
{
	Ender_Event_Callback callback;
	void *data;
} Ender_Listener;

typedef struct _Ender_New_Listener
{
	Ender_New_Callback callback;
	void *data;
} Ender_New_Listener;

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
	void *data;
} Ender_Element_Property;

struct _Ender_Element
{
	EINA_MAGIC
	Ender_Descriptor *descriptor;
	Enesim_Renderer *renderer;
	Eina_Hash *listeners;
	Ender_Element *parent;
	Eina_Hash *properties;
};

static Ender_Element * _ender_element_new(const char *name, const char *ns_name)
{
	Ender_Element *ender;
	Ender_New_Listener *listener;
	Enesim_Renderer *renderer;
	Ender_Descriptor *desc;
	Eina_List *l;

	desc = ender_descriptor_find_with_namespace(name, ns_name);
	DBG("Creating new ender \"%s\" for namesapce \"%s\"", name, ns_name);
	if (!desc)
	{
		ERR("No such descriptor for name \"%s.%s\"", ns_name, name);
		return NULL;
	}
	if (!desc->create)
	{
		ERR("The descriptor for name \"%s.%s\" does not have a creator", ns_name, name);
		return NULL;
	}

	renderer = desc->create();
	if (!renderer)
	{
		ERR("For some reason the creator for \"%s.%s\" failed", ns_name, name);
		return NULL;
	}

	ender = calloc(1, sizeof(Ender_Element));
	EINA_MAGIC_SET(ender, ENDER_MAGIC);
	ender->renderer = renderer;
	ender->descriptor = desc;
	ender->listeners = eina_hash_string_superfast_new(NULL);
	ender->properties = eina_hash_string_superfast_new(NULL);
	enesim_renderer_private_set(renderer, "ender", ender);
	/* call the constructor callback */
	EINA_LIST_FOREACH(_new_callbacks, l, listener)
	{
		listener->callback(ender, listener->data);
	}

	return ender;

}

static void _property_is_relative_cb(Ender_Property *prop, void *data)
{
	Ender_Element_Property_List_Data *new_data = data;

	if (ender_property_is_relative(prop))
		new_data->cb(prop, new_data->data);
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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_element_parent_set(Ender_Element *e, Ender_Element *parent)
{
	e->parent = parent;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Create a new ender
 * @param name
 * @return
 */
EAPI Ender_Element * ender_element_new(const char *name)
{
	return ender_element_new_with_namespace(name, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_element_new_with_namespace(const char *name, const char *ns_name)
{
	return _ender_element_new(name, ns_name);
}

/**
 * Delete an ender
 */
EAPI void ender_element_delete(Ender_Element *e)
{
	ENDER_MAGIC_CHECK(e);
	enesim_renderer_unref(e->renderer);
	free(e);
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
		ec = ender_container_compound_get(ec, 0);

		ENDER_VALUE_COLLECT(v, ec, var_args);
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
		ec = ender_container_compound_get(ec, 0);

		ENDER_VALUE_COLLECT(v, ec, var_args);
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
EAPI void ender_element_value_get_simple(Ender_Element *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_descriptor_property_get(e->descriptor, name);
	if (!prop) return;
	ender_property_element_value_get(prop, e, value);
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

		ENDER_VALUE_COLLECT(v, ec, var_args);
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
	eprop->data = data;

	prop = ender_property_new(name, ec,
			_property_get,
			_property_set,
			_property_add,
			_property_remove,
			_property_clear,
			relative, eprop);
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

	ENDER_MAGIC_CHECK(e);
	/* first check dynamic properties */
	prop = eina_hash_find(e->properties, name);
	if (prop) return prop;
	/* now the static properties */
	prop = ender_descriptor_property_get(e->descriptor, name);
	if (prop) return prop;
	/* now check relative proprties */
	if (!e->parent)
	{
		WRN("Property %s not found and it has no parent", name);
		return NULL;
	}

	prop = ender_descriptor_property_get(e->parent->descriptor, name);
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
	Ender_Descriptor *desc;

	ENDER_MAGIC_CHECK(e);

	/* we first list all the local properties */
	desc = e->descriptor;
	/* now the parent descriptor */
	do
	{
		ender_descriptor_property_list(desc, cb, data);
	}
	while ((desc = ender_descriptor_parent(desc)));
	/* now let's lis the relative properties from the parent */
	if (!e->parent) return;
	new_data.data = data;
	new_data.cb = cb;
	ender_descriptor_property_list(e->parent->descriptor, _property_is_relative_cb, &new_data);
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
		ENDER_VALUE_COLLECT(v, ec, var_args);
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
 * Get the enesim renderer associated with an ender
 * @param e
 * @return
 */
EAPI Enesim_Renderer * ender_element_renderer_get(Ender_Element *e)
{
	ENDER_MAGIC_CHECK(e);
	return e->renderer;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_element_parent_get(Ender_Element *e)
{
	ENDER_MAGIC_CHECK(e);
	return e->parent;
}

/**
 * Get the ender associated with an enesim renderer
 * @param r
 * @return
 */
EAPI Ender_Element * ender_element_renderer_from(Enesim_Renderer *r)
{
	return enesim_renderer_private_get(r, "ender");
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_event_listener_add(Ender_Element *e, const char *event_name,
		Ender_Event_Callback cb, void *data)
{
	Ender_Listener *listener;
	Eina_List *listeners, *tmp;

	ENDER_MAGIC_CHECK(e);
	listener = calloc(1, sizeof(Ender_Listener));
	listener->callback = cb;
	listener->data = data;

	listeners = eina_hash_find(e->listeners, event_name);
	tmp = eina_list_append(listeners, listener);
	if (!listeners)
	{
		eina_hash_add(e->listeners, event_name, tmp);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_event_listener_remove(Ender_Element *e, const char *name,
		Ender_Event_Callback cb)
{
	ENDER_MAGIC_CHECK(e);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_event_dispatch(Ender_Element *e, const char *event_name, void *event_data)
{
	Ender_Listener *listener;
	Eina_List *listeners;
	Eina_List *l;

	ENDER_MAGIC_CHECK(e);
	listeners = eina_hash_find(e->listeners, event_name);
	if (!listeners) return;

	EINA_LIST_FOREACH(listeners, l, listener)
	{
		listener->callback(e, event_name, event_data, listener->data);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_new_listener_add(Ender_New_Callback cb, void *data)
{
	Ender_New_Listener *listener;

	listener = calloc(1, sizeof(Ender_New_Listener));
	listener->callback = cb;
	listener->data = data;

	_new_callbacks = eina_list_append(_new_callbacks, listener);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_element_new_listener_remove(Ender_New_Callback cb, void *data)
{
	Ender_New_Listener *listener;
	Eina_List *l;

	EINA_LIST_FOREACH(_new_callbacks, l, listener)
	{
		if (listener->callback == cb && listener->data == data)
		{
			_new_callbacks = eina_list_remove(l, listener);
			break;
		}
	}
}
