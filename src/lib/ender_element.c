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

#define ENDER_VALUE_COLLECT(v, cnt, args)				\
	{								\
		v.container = cnt;					\
		switch (cnt->type)					\
		{							\
			case ENDER_ARGB:				\
			case ENDER_UINT32:				\
			case ENDER_INT32:				\
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

static Eina_List *_new_callbacks = NULL;

typedef void (*Ender_Value_Accessor)(Ender_Value *v, Ender_Accessor acc,
		Ender *e, Ender *parent);

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

struct _Ender
{
	EINA_MAGIC;
	Ender_Descriptor *descriptor;
	Enesim_Renderer *renderer;
	Eina_Hash *listeners;
	Ender *parent;
};
/*----------------------------------------------------------------------------*
 *                           uint32 / in32 / argb                             *
 *----------------------------------------------------------------------------*/
static void _ender_int32_set(Ender_Value *v, Ender_Setter set, Ender *e,
		Ender *parent)
{
	set(e->renderer, v->data.i32);
}

static void _ender_relative_int32_set(Ender_Value *v, Ender_Setter set,
		Ender *e, Ender *parent)
{
	set(parent->renderer, e, v->data.i32);
}
/*----------------------------------------------------------------------------*
 *                                   double                                   *
 *----------------------------------------------------------------------------*/
static void _ender_double_set(Ender_Value *v, Ender_Setter set, Ender *e,
		Ender *parent)
{
	set(e->renderer, v->data.d);
}

static void _ender_relative_double_set(Ender_Value *v, Ender_Setter set, Ender *e,
		Ender *parent)
{
	set(parent->renderer, e, v->data.d);
}
/*----------------------------------------------------------------------------*
 *                  string / surface / struct / matrix / renderer             *
 *----------------------------------------------------------------------------*/
static void _ender_pointer_set(Ender_Value *v, Ender_Setter set,
		Ender *e, Ender *parent)
{
	set(e->renderer, v->data.ptr);
}

static void _ender_relative_pointer_set(Ender_Value *v, Ender_Setter set,
		Ender *e, Ender *parent)
{
	set(parent->renderer, e, v->data.ptr);
}
/*----------------------------------------------------------------------------*
 *                                   ender                                    *
 *----------------------------------------------------------------------------*/
static void _ender_ender_set(Ender_Value *v, Ender_Setter set,
		Ender *e, Ender *parent)
{
	Ender *child;

	child = v->data.ptr;
	/* TODO check if it is an ender */
	set(e->renderer, v->data.ptr);
	child->parent = e;
}

static void _ender_relative_ender_set(Ender_Value *v, Ender_Setter set,
		Ender *e, Ender *parent)
{
	set(parent->renderer, e, v->data.ptr);
}

static Ender_Value_Accessor _accessors[ENDER_PROPERTY_TYPES] = {
	[ENDER_UINT32] = _ender_int32_set,
	[ENDER_INT32] = _ender_int32_set,
	[ENDER_DOUBLE] = _ender_double_set,
	[ENDER_ARGB] = _ender_int32_set,
	[ENDER_STRING] = _ender_pointer_set,
	[ENDER_MATRIX] = _ender_pointer_set,
	[ENDER_RENDERER] = _ender_pointer_set,
	[ENDER_SURFACE] = _ender_pointer_set,
	[ENDER_ENDER] = _ender_ender_set,
	[ENDER_LIST] = _ender_pointer_set,
	[ENDER_STRUCT] = _ender_pointer_set,
};

static Ender_Value_Accessor _relative_accessors[ENDER_PROPERTY_TYPES] = {
	[ENDER_UINT32] = _ender_relative_int32_set,
	[ENDER_INT32] = _ender_relative_int32_set,
	[ENDER_DOUBLE] = _ender_relative_double_set,
	[ENDER_ARGB] = _ender_relative_int32_set,
	[ENDER_STRING] = _ender_relative_pointer_set,
	[ENDER_MATRIX] = _ender_relative_pointer_set,
	[ENDER_RENDERER] = _ender_relative_pointer_set,
	[ENDER_SURFACE] = _ender_relative_pointer_set,
	[ENDER_ENDER] = _ender_relative_ender_set,
	[ENDER_LIST] = _ender_relative_pointer_set,
	[ENDER_STRUCT] = _ender_relative_pointer_set,
};

static void _value_set(Ender_Value *v, Ender_Setter set, Ender *e,
		Ender *parent, Eina_Bool relative)
{
	Ender_Event_Mutation event_data;

	if (relative)
	{
		if (parent) _relative_accessors[v->container->type](v, set, e, parent);
		else WRN("Trying to set a relative property with no parent");
	}
	else
	{
		_accessors[v->container->type](v, set, e, NULL);
	}
	ender_event_dispatch(e, "Mutation", &event_data);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* placeholder to initialize all the C99 structs */
void ender_element_init(void)
{

}

void ender_element_shutdown(void)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Create a new ender
 * @param name
 * @return
 */
EAPI Ender * ender_element_new(const char *name)
{
	Ender *ender;
	Ender_Descriptor *desc;
	Ender_New_Listener *listener;
	Enesim_Renderer *renderer;
	Eina_List *l;

	DBG("Creating new ender \"%s\"", name);
	desc = ender_descriptor_find(name);
	if (!desc)
	{
		ERR("No such descriptor for name \"%s\"", name);
		return NULL;
	}
	if (!desc->create)
	{
		ERR("The descriptor for name \"%s\" does not have a creator", name);
		return NULL;
	}

	renderer = desc->create();
	if (!renderer)
	{
		ERR("For some reason the creator for \"%s\" failed", name);
		return NULL;
	}

	ender = calloc(1, sizeof(Ender));
	EINA_MAGIC_SET(ender, ENDER_MAGIC);
	ender->renderer = renderer;
	ender->descriptor = desc;
	ender->listeners = eina_hash_string_superfast_new(NULL);
	enesim_renderer_private_set(renderer, "ender", ender);
	/* call the constructor callback */
	EINA_LIST_FOREACH(_new_callbacks, l, listener)
	{
		listener->callback(ender, listener->data);
	}

	return ender;
}

/**
 * Delete an ender
 */
EAPI void ender_element_delete(Ender *e)
{
	ENDER_MAGIC_CHECK(e);
	enesim_renderer_delete(e->renderer);
	free(e);
}

EAPI Ender_Descriptor * ender_element_descriptor_get(Ender *e)
{
	return e->descriptor;
}

/**
 *
 */
EAPI const char * ender_element_name_get(Ender *e)
{
	ENDER_MAGIC_CHECK(e);
	return ender_descriptor_name_get(e->descriptor);
}

/**
 *
 */
EAPI void ender_element_value_add_valist(Ender *e, const char *name, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (name)
	{
		Ender_Property *prop;
		Ender_Container *ec;
		Ender_Value v;

		prop = ender_descriptor_property_get(e->descriptor, name);
		if (!prop) break;
		if (prop->prop->type != ENDER_LIST) break;
		ec = ender_container_compound_get(prop->prop, 0);

		ENDER_VALUE_COLLECT(v, ec, var_args);
		_value_set(&v, prop->add, e, e->parent, prop->relative);
		name = va_arg(var_args, char *);
	} 
}

/**
 *
 */
EAPI void ender_element_value_add(Ender *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, name);
	ender_element_value_add_valist(e, name, ap);
	va_end(ap);
}

/**
 *
 */
EAPI void ender_element_value_add_simple(Ender *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_element_property_get(e, name);
	if (!prop) return;
	_value_set(value, prop->add, e, e->parent, prop->relative);
}

/**
 *
 */
EAPI void ender_element_value_remove_valist(Ender *e, const char *name, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (name)
	{
		Ender_Property *prop;
		Ender_Container *ec;
		Ender_Value v;

		prop = ender_descriptor_property_get(e->descriptor, name);
		if (!prop) break;
		if (prop->prop->type != ENDER_LIST) break;
		ec = ender_container_compound_get(prop->prop, 0);

		ENDER_VALUE_COLLECT(v, ec, var_args);
		_value_set(&v, prop->remove, e, e->parent, prop->relative);
		name = va_arg(var_args, char *);
	} 
}

/**
 *
 */
EAPI void ender_element_value_remove(Ender *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, name);
	ender_element_value_remove_valist(e, name, ap);
	va_end(ap);
}

/**
 *
 */
EAPI void ender_element_value_remove_simple(Ender *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_element_property_get(e, name);
	if (!prop) return;
	_value_set(value, prop->remove, e, e->parent, prop->relative);
}

/**
 *
 */
EAPI void ender_element_value_clear(Ender *e, const char *name)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	prop = ender_descriptor_property_get(e->descriptor, name);
	if (!prop) return;
	if (prop->prop->type != ENDER_LIST) return;
	if (prop->clear) prop->clear(e->renderer);
}

/**
 *
 */
EAPI void ender_element_value_get_valist(Ender *e, const char *name, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);
	while (name)
	{
		Ender_Property *prop;
		uint32_t *u32;
		int32_t *i32;
		double *d;
		Enesim_Color *color;
		char **string;
		Enesim_Matrix *matrix;
		Enesim_Renderer **renderer;
		Eina_List *list;
		Enesim_Surface *surface;
		Ender **ender;

		prop = ender_descriptor_property_get(e->descriptor, name);
		if (!prop) return;

		switch (prop->prop->type)
		{
			case ENDER_UINT32:
			u32 = va_arg(var_args, uint32_t *);
			prop->get(e->renderer, u32);
			break;

			case ENDER_INT32:
			i32 = va_arg(var_args, int32_t *);
			prop->get(e->renderer, i32);
			break;

			case ENDER_DOUBLE:
			d = va_arg(var_args, double *);
			prop->get(e->renderer, d);
			break;

			case ENDER_ARGB:
			color = va_arg(var_args, Enesim_Color *);
			prop->get(e->renderer, color);
			break;

			case ENDER_STRING:
			string = va_arg(var_args, char **);
			prop->get(e->renderer, string);
			break;

			case ENDER_MATRIX:
			matrix = va_arg(var_args, Enesim_Matrix *);
			prop->get(e->renderer, matrix);
			break;

			case ENDER_RENDERER:
			renderer = va_arg(var_args, Enesim_Renderer **);
			prop->get(e->renderer, renderer);
			break;

			case ENDER_ENDER:
			ender = va_arg(var_args, Ender **);
			prop->get(e->renderer, ender);
			break;

			default:
			ERR("Unsupported data type %d", prop->prop->type);
			break;
			
		}
		name = va_arg(var_args, char *);
	}
}


/**
 *
 */
EAPI void ender_element_value_get(Ender *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);
	va_start(ap, name);
	ender_element_value_get_valist(e, name, ap);
	va_end(ap);
}

/**
 *
 */
EAPI void ender_element_value_get_simple(Ender *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_descriptor_property_get(e->descriptor, name);
	if (!prop) return;
	switch (prop->prop->type)
	{
		case ENDER_MATRIX:
		prop->get(e->renderer, &value);
		break;

		default:
		prop->get(e->renderer, value);
		break;
	}
}

/**
 *
 */
EAPI void ender_element_value_set_valist(Ender *e, const char *name, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (name)
	{
		Ender_Property *prop;
		Ender_Value v;

		prop = ender_element_property_get(e, name);
		if (!prop) return;

		ENDER_VALUE_COLLECT(v, prop->prop, var_args);
		_value_set(&v, prop->set, e, e->parent, prop->relative);
		name = va_arg(var_args, char *);
	} 
}

/**
 *
 */
EAPI void ender_element_value_set(Ender *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, name);
	ender_element_value_set_valist(e, name, ap);
	va_end(ap);
}

/**
 *
 */
EAPI void ender_element_value_set_simple(Ender *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_element_property_get(e, name);
	if (!prop) return;
	_value_set(value, prop->set, e, e->parent, prop->relative);
}

/**
 *
 */
EAPI Ender_Property * ender_element_property_get(Ender *e, const char *name)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	prop = ender_descriptor_property_get(e->descriptor, name);
	if (prop) return prop;
	/* now check relative proprties */
	if (!e->parent) return NULL;

	prop = ender_descriptor_property_get(e->parent->descriptor, name);
	if (!prop) return NULL;
	if (!prop->relative) return NULL;

	return prop;
}

/**
 *
 */
EAPI void ender_element_property_value_set_valist(Ender *e, Ender_Property *prop, va_list var_args)
{
	ENDER_MAGIC_CHECK(e);

	while (prop)
	{
		Ender_Value v;

		ENDER_VALUE_COLLECT(v, prop->prop, var_args);
		_value_set(&v, prop->set, e, e->parent, prop->relative);
		prop = va_arg(var_args, Ender_Property *);
	} 
}

/**
 *
 */
EAPI void ender_element_property_value_set(Ender *e, Ender_Property *prop, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);

	va_start(ap, prop);
	ender_element_property_value_set_valist(e, prop, ap);
	va_end(ap);
}

/**
 *
 */
EAPI void ender_element_property_value_set_simple(Ender *e, Ender_Property *prop, Ender_Value *value)
{
	ENDER_MAGIC_CHECK(e);

	_value_set(value, prop->set, e, e->parent, prop->relative);
}

/**
 * Get the enesim renderer associated with an ender
 * @param e
 * @return
 */
EAPI Enesim_Renderer * ender_element_renderer_get(Ender *e)
{
	ENDER_MAGIC_CHECK(e);
	return e->renderer;
}

/**
 *
 */
EAPI Ender * ender_element_parent_get(Ender *e)
{
	ENDER_MAGIC_CHECK(e);
	return e->parent;
}

/**
 * Get the ender associated with an enesim renderer
 * @param r
 * @return
 */
EAPI Ender * ender_element_renderer_from(Enesim_Renderer *r)
{
	return enesim_renderer_private_get(r, "ender");
}

/**
 *
 */
EAPI void ender_event_listener_add(Ender *e, const char *event_name,
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
 *
 */
EAPI void ender_event_listener_remove(Ender *e, const char *name,
		Ender_Event_Callback cb)
{
	ENDER_MAGIC_CHECK(e);
	Ender_Listener *listener;
}

/**
 *
 */
EAPI void ender_event_dispatch(Ender *e, const char *event_name, void *event_data)
{
	Ender_Listener *listener;
	Eina_List *listeners;
	Eina_Iterator *it;

	ENDER_MAGIC_CHECK(e);
	listeners = eina_hash_find(e->listeners, event_name);
	if (!listeners) return;

	it = eina_list_iterator_new(listeners);
	while (eina_iterator_next(it, (void **)&listener))
	{
		listener->callback(e, event_name, event_data, listener->data);
	}
	eina_iterator_free(it);
}

/**
 *
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
 *
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
