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
typedef struct _Ender_Property
{
	char *name;
	Ender_Getter get;
	Ender_Setter set;
	Ender_Property_Type type;
} Ender_Property;

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

static Eina_Hash *_descriptors = NULL;

static Ender_Property * _property_get(Ender_Descriptor *e, const char *name)
{
	Ender_Property *prop;

	prop = eina_hash_find(e->properties, name);
	if (prop) return prop;
	if (!e->parent) return NULL;

	return _property_get(e->parent, name);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
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
	printf("descriptor %s added\n", name);

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

void ender_descriptor_property_add(Ender_Descriptor *edesc, const char *name,
	Ender_Property_Type t, Ender_Getter get, Ender_Setter set)
{
	Ender_Property *prop;

	prop = eina_hash_find(edesc->properties, name);
	if (prop) return;

	/* get the getter/setter */
	prop = malloc(sizeof(Ender_Property));
	prop->name = strdup(name);
	prop->type = t;
	prop->get = get;
	prop->set = set;
	eina_hash_add(edesc->properties, name, prop);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void ender_init(void)
{
	eina_init();
	enesim_init();
	_descriptors = eina_hash_string_superfast_new(NULL);
	ender_parser_init();
}

EAPI void ender_shutdown(void)
{
	ender_parser_shutdown();
	enesim_shutdown();
	eina_shutdown();
	//eina_hash_delete(_descriptors);
}

EAPI Ender * ender_new(const char *name)
{
	Ender *ender;
	Ender_Descriptor *desc;

	desc = eina_hash_find(_descriptors, name);
	if (!desc) return NULL;
	if (!desc->create) return NULL;

	ender = malloc(sizeof(Ender));
	ender->renderer = desc->create();
	ender->descriptor = desc;

	return ender;
}

EAPI const char * ender_name_get(Ender *e)
{
	return ender_descriptor_name_get(e->descriptor);
}

EAPI Eina_Bool ender_property_get(Ender *e, char *name, Ender_Property_Type *type)
{
	Ender_Property *prop;

	prop = _property_get(e->descriptor, name);
	if (!prop) return EINA_FALSE;

	*type = prop->type;
	return EINA_TRUE;
}

EAPI void ender_value_get(Ender *e, ...)
{
	va_list ap;
	char *name;
	
	va_start(ap, e);
	while ((name = va_arg(ap, char *)))
	{
		Ender_Property *prop;
		
		prop = _property_get(e->descriptor, name);
		if (!prop) return;

		switch (prop->type)
		{
			case ENDER_UINT32:
			case ENDER_INT32:
			case ENDER_FLOAT:
			case ENDER_DOUBLE:
			case ENDER_COLOR:
			case ENDER_STRING:
			case ENDER_MATRIX:
			break;
		}
	}
}

EAPI void ender_value_set(Ender *e, ...)
{
	va_list ap;
	char *name;
	
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

		prop = _property_get(e->descriptor, name);
		if (!prop) return;

		switch (prop->type)
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
			printf("setting float %g\n", d);
			prop->set(e->renderer, d);
			break;

			case ENDER_DOUBLE:
			d = va_arg(ap, double);
			printf("setting double %g\n", d);
			prop->set(e->renderer, d);
			break;

			case ENDER_COLOR:
			color = va_arg(ap, Enesim_Color);
			printf("setting color %08x\n", color);
			prop->set(e->renderer, color);
			break;

			case ENDER_STRING:
			string = va_arg(ap, char *);
			prop->set(e->renderer, string);
			break;

			case ENDER_MATRIX:
			matrix = va_arg(ap, Enesim_Matrix *);
			prop->set(e->renderer, matrix);
			break;
		}
	}
}

EAPI Enesim_Renderer * ender_renderer_get(Ender *e)
{
	return e->renderer;
}

