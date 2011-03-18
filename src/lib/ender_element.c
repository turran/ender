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
#define ENDER_MAGIC_CHECK(d)\
	do {\
		if (!EINA_MAGIC_CHECK(d, ENDER_MAGIC))\
			EINA_MAGIC_FAIL(d, ENDER_MAGIC);\
	} while(0)

typedef struct _Ender_Listener
{
	Ender_Event_Callback callback;
	void *data;
} Ender_Listener;

struct _Ender
{
	EINA_MAGIC;
	Ender_Descriptor *descriptor;
	Enesim_Renderer *renderer;
	Eina_Hash *listeners;
	Ender *parent;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
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
	Enesim_Renderer *renderer;

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

	ender = malloc(sizeof(Ender));
	EINA_MAGIC_SET(ender, ENDER_MAGIC);
	ender->renderer = renderer;
	ender->descriptor = desc;
	ender->listeners = eina_hash_string_superfast_new(NULL);
	enesim_renderer_private_set(renderer, "ender", ender);

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
EAPI void ender_element_value_add(Ender *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);
	va_start(ap, name);
	do
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
		Ender *ender;

		prop = ender_descriptor_property_get(e->descriptor, name);
		if (!prop) break;
		if (prop->prop->type != ENDER_LIST) break;
		if (prop->add) prop->add(e->renderer, va_arg(ap, void *));
	} while ((name = va_arg(ap, char *)));
	va_end(ap);
}

/**
 *
 */
EAPI void ender_element_value_clear(Ender *e, const char *name, ...)
{
	va_list ap;

	ENDER_MAGIC_CHECK(e);
	va_start(ap, name);
	do
	{
		Ender_Property *prop;
		prop = ender_descriptor_property_get(e->descriptor, name);
		if (!prop) break;
		if (prop->prop->type != ENDER_LIST) break;
		if (prop->clear) prop->clear(e->renderer);
	} while ((name = va_arg(ap, char *)));
	va_end(ap);
}

/**
 *
 */
EAPI void ender_element_value_get(Ender *e, ...)
{
	va_list ap;
	char *name;

	ENDER_MAGIC_CHECK(e);
	va_start(ap, e);
	while ((name = va_arg(ap, char *)))
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
			u32 = va_arg(ap, uint32_t *);
			prop->get(e->renderer, u32);
			break;

			case ENDER_INT32:
			i32 = va_arg(ap, int32_t *);
			prop->get(e->renderer, i32);
			break;

			case ENDER_DOUBLE:
			d = va_arg(ap, double *);
			prop->get(e->renderer, d);
			break;

			case ENDER_ARGB:
			color = va_arg(ap, Enesim_Color *);
			prop->get(e->renderer, color);
			break;

			case ENDER_STRING:
			string = va_arg(ap, char **);
			prop->set(e->renderer, string);
			break;

			case ENDER_MATRIX:
			matrix = va_arg(ap, Enesim_Matrix *);
			prop->get(e->renderer, matrix);
			break;

			case ENDER_RENDERER:
			renderer = va_arg(ap, Enesim_Renderer **);
			prop->set(e->renderer, renderer);
			break;

			case ENDER_ENDER:
			ender = va_arg(ap, Ender **);
			prop->set(e->renderer, ender);
			break;
		}
	}
	va_end(ap);
}

/**
 *
 */
EAPI void ender_element_value_set(Ender *e, ...)
{
	va_list ap;
	char *name;

	ENDER_MAGIC_CHECK(e);
	va_start(ap, e);
	while ((name = va_arg(ap, char *)))
	{
		Ender_Property *prop;
		uint32_t u32;
		int32_t i32;
		double d;
		Enesim_Color color;
		char *string;
		Enesim_Matrix *matrix;
		Enesim_Renderer *renderer;
		Eina_List *list;
		Enesim_Surface *surface;
		Ender *ender;

		prop = ender_descriptor_property_get(e->descriptor, name);
		if (!prop) return;

		switch (prop->prop->type)
		{
			case ENDER_UINT32:
			u32 = va_arg(ap, uint32_t);
			prop->set(e->renderer, u32);
			break;

			case ENDER_INT32:
			i32 = va_arg(ap, int32_t);
			prop->set(e->renderer, i32);
			break;

			case ENDER_DOUBLE:
			d = va_arg(ap, double);
			prop->set(e->renderer, d);
			break;

			case ENDER_ARGB:
			color = va_arg(ap, Enesim_Color);
			prop->set(e->renderer, color);
			break;

			case ENDER_STRING:
			string = va_arg(ap, char *);
			prop->set(e->renderer, string);
			break;

			case ENDER_SURFACE:
			surface = va_arg(ap, Enesim_Surface *);
			prop->set(e->renderer, surface);
			break;

			case ENDER_MATRIX:
			matrix = va_arg(ap, Enesim_Matrix *);
			prop->set(e->renderer, matrix);
			break;

			case ENDER_RENDERER:
			renderer = va_arg(ap, Enesim_Renderer *);
			prop->set(e->renderer, renderer);
			break;

			case ENDER_ENDER:
			ender = va_arg(ap, Ender *);
			ender->parent = e;
			prop->set(e->renderer, ender);
			break;

			case ENDER_LIST:
			/* TODO whenever we set a list of enders, the parent must be set
			 * too
			 */
			list = va_arg(ap, Eina_List *);
			prop->set(e->renderer, list);
			break;
		}
	}
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
EAPI void ender_element_value_set_simple(Ender *e, const char *name, Ender_Value *value)
{
	Ender_Property *prop;

	ENDER_MAGIC_CHECK(e);
	if (!value) return;

	prop = ender_descriptor_property_get(e->descriptor, name);
	if (!prop) return;
	switch (prop->prop->type)
	{
		case ENDER_MATRIX:
		prop->set(e->renderer, value);
		break;

		case ENDER_ENDER:
		value->ender->parent = e;
		prop->set(e->renderer, *value);
		break;

		default:
		prop->set(e->renderer, *value);
		break;
	}
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
	if (!prop->relative) return NULL;

	return prop;
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
