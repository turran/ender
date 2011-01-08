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
/* TODO
 * - Add introspection functions: a way to know the properties, the
 * values, the parent, etc.
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Ender_Descriptor_Property
{
	char *name;
	Ender_Getter get;
	Ender_Setter set;
	Ender_Property *prop;
};

struct _Ender_Descriptor
{
	char *name;
	Ender_Descriptor *parent;
	Ender_Creator create;
	Eina_Hash *properties;
};

struct _Ender
{
	Ender_Descriptor *descriptor;
	Enesim_Renderer *renderer;
};

struct _Ender_Property
{
	Ender_Property_Type type;
	Eina_Array *sub;
};

static Eina_Hash *_descriptors = NULL;

static Ender_Descriptor_Property * _property_get(Ender_Descriptor *e, const char *name)
{
	Ender_Descriptor_Property *prop;

	prop = eina_hash_find(e->properties, name);
	if (prop) return prop;
	if (!e->parent) return NULL;

	return _property_get(e->parent, name);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int ender_log_dom = -1;

Ender_Descriptor * ender_descriptor_register(const char *name, Ender_Creator creator,
		Ender_Descriptor *parent)
{
	Ender_Descriptor *desc;

	desc = eina_hash_find(_descriptors, name);
	if (desc)
	{
		/* already exists */
		return desc;
	}

	desc = malloc(sizeof(Ender_Descriptor));
	desc->name = strdup(name);
	desc->parent = parent;
	desc->create = creator;
	desc->properties =  eina_hash_string_superfast_new(NULL);

	eina_hash_add(_descriptors, name, desc);
	DBG("Descriptor %s added", name);

	return desc;
}

Ender_Descriptor * ender_descriptor_get(const char *name)
{
	return eina_hash_find(_descriptors, name);
}

const char * ender_descriptor_name_get(Ender_Descriptor *edesc)
{
	return edesc->name;
}

void ender_descriptor_unregister(Ender_Descriptor *edesc)
{
	/* TODO remove every property */
	//eina_hash_remove(_descriptors, ..);
}

Ender_Property * ender_property_new(Ender_Property_Type t)
{
	Ender_Property *prop;

	prop = malloc(sizeof(Ender_Property));
	prop->type = t;
	prop->sub = NULL;
	switch (t)
	{
		case ENDER_LIST:
		prop->sub = eina_array_new(1);
		break;

		default:
		break;
	}
	return prop;
}

void ender_property_delete(Ender_Property *d)
{
	if (d->sub)
	{
		/* call the delete for each descriptor */
	}
	free(d);
}

void ender_property_add(Ender_Property *d, Ender_Property *sub)
{
	//printf("adding sub property %p to %p\n", sub, d);
	if (!d->sub) return;
	//printf("pushing\n");
	eina_array_push(d->sub, sub);
}

void ender_descriptor_property_add(Ender_Descriptor *edesc, const char *name,
	Ender_Property *prop, Ender_Getter get, Ender_Setter set)
{
	Ender_Descriptor_Property *dprop;

	dprop = eina_hash_find(edesc->properties, name);
	if (dprop)
	{
		WRN("Property %s already found on %s. Not adding it", name, edesc->name);
		return;
	}

	/* get the getter/setter */
	dprop = malloc(sizeof(Ender_Descriptor_Property));
	dprop->name = strdup(name);
	dprop->get = get;
	dprop->set = set;
	dprop->prop = prop;
	eina_hash_add(edesc->properties, name, dprop);
	DBG("Property %s (%d) added to %s", name, prop->type, edesc->name);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Initialize the ender library
 */
EAPI void ender_init(void)
{
	eina_init();
	ender_log_dom = eina_log_domain_register("ender", NULL);
	enesim_init();
	_descriptors = eina_hash_string_superfast_new(NULL);
	ender_parser_init();
}

/**
 * Shutdown the ender library
 */
EAPI void ender_shutdown(void)
{
	ender_parser_shutdown();
	enesim_shutdown();
	eina_log_domain_unregister(ender_log_dom);
	eina_shutdown();
	//eina_hash_delete(_descriptors);
}

/**
 *
 */
EAPI void ender_list(Ender_List_Callback cb, void *data)
{
	Eina_Iterator *it;
	char *name;

	it = eina_hash_iterator_key_new(_descriptors);
	while (eina_iterator_next(it, (void **)&name))
	{
		cb(name, data);
	}
	eina_iterator_free(it);
}

/**
 *
 */
EAPI Eina_Bool ender_exists(const char *name)
{
	Ender_Descriptor *desc;

	desc = eina_hash_find(_descriptors, name);
	if (desc) return EINA_TRUE;
	else return EINA_FALSE;
}
/**
 *
 */
EAPI Ender * ender_new(const char *name)
{
	Ender *ender;
	Ender_Descriptor *desc;

	DBG("Creating new ender \"%s\"", name);
	desc = eina_hash_find(_descriptors, name);
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

	ender = malloc(sizeof(Ender));
	ender->renderer = desc->create();
	ender->descriptor = desc;

	return ender;
}

/**
 *
 */
EAPI void ender_delete(Ender *e)
{
	enesim_renderer_delete(e->renderer);
	free(e);
}

/**
 *
 */
EAPI const char * ender_name_get(Ender *e)
{
	return ender_descriptor_name_get(e->descriptor);
}

/**
 *
 */
EAPI Ender_Property * ender_property_get(Ender *e, const char *name)
{
	Ender_Descriptor_Property *dprop;

	dprop = _property_get(e->descriptor, name);
	if (!dprop) return NULL;

	return dprop->prop;
}
/**
 *
 */
EAPI void ender_property_list(Ender *e, Ender_Property_List_Callback cb, void *data)
{
	Eina_Iterator *it;
	Ender_Descriptor *desc;
	char *name;

	desc = e->descriptor;
	it = eina_hash_iterator_key_new(desc->properties);
	while (eina_iterator_next(it, (void **)&name))
	{
		cb(e, name, data);
	}
	eina_iterator_free(it);
}

/**
 *
 */
EAPI Ender_Property_Type ender_property_type(Ender_Property *p)
{
	return p->type;
}

/**
 *
 */
EAPI const Eina_Array * ender_property_sub(Ender_Property *p)
{
	return p->sub;
}

/**
 *
 */
EAPI void ender_value_get(Ender *e, ...)
{
	va_list ap;
	char *name;

	va_start(ap, e);
	while ((name = va_arg(ap, char *)))
	{
		Ender_Descriptor_Property *prop;

		prop = _property_get(e->descriptor, name);
		if (!prop) return;

		switch (prop->prop->type)
		{
			case ENDER_UINT32:
			case ENDER_INT32:
			case ENDER_FLOAT:
			case ENDER_DOUBLE:
			case ENDER_COLOR:
			case ENDER_STRING:
			case ENDER_MATRIX:
			case ENDER_RENDERER:
			break;
		}
	}
}

/**
 *
 */
EAPI void ender_value_set(Ender *e, ...)
{
	va_list ap;
	char *name;

	va_start(ap, e);
	while ((name = va_arg(ap, char *)))
	{
		Ender_Descriptor_Property *prop;
		uint32_t u32;
		int32_t i32;
		double d;
		Enesim_Color color;
		char *string;
		Enesim_Matrix *matrix;
		Enesim_Renderer *renderer;
		Eina_List *list;
		Enesim_Surface *surface;

		prop = _property_get(e->descriptor, name);
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

			case ENDER_FLOAT:
			d = va_arg(ap, double);
			prop->set(e->renderer, d);
			break;

			case ENDER_DOUBLE:
			d = va_arg(ap, double);
			prop->set(e->renderer, d);
			break;

			case ENDER_COLOR:
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

			case ENDER_LIST:
			list = va_arg(ap, Eina_List *);
			prop->set(e->renderer, list);
			break;
		}
	}
}

/**
 *
 */
EAPI Enesim_Renderer * ender_renderer_get(Ender *e)
{
	return e->renderer;
}

