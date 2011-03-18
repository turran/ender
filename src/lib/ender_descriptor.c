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
 * - For compound types (lists, arrays) add a way to add/remove/clear
 * the property
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Hash *_descriptors = NULL;

static Ender_Property * _property_get(Ender_Descriptor *e, const char *name);

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
		Ender_Descriptor *parent, Ender_Type type)
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
	desc->type = type;
	desc->properties = eina_hash_string_superfast_new(NULL);

	eina_hash_add(_descriptors, name, desc);
	DBG("Descriptor %s added", name);

	return desc;
}

void ender_descriptor_unregister(Ender_Descriptor *edesc)
{
	/* TODO remove every property */
	//eina_hash_remove(_descriptors, ..);
}

void ender_descriptor_property_add(Ender_Descriptor *edesc, const char *name,
	Ender_Property_Container *prop, Ender_Getter get, Ender_Setter set,
	Ender_Add add, Ender_Remove remove, Ender_Clear clear,
	Eina_Bool relative)
{
	Ender_Property *dprop;

	dprop = eina_hash_find(edesc->properties, name);
	if (dprop)
	{
		WRN("Property %s already found on %s. Not adding it", name, edesc->name);
		return;
	}

	/* get the getter/setter */
	dprop = malloc(sizeof(Ender_Property));
	dprop->name = strdup(name);
	dprop->get = get;
	dprop->set = set;
	dprop->add = add;
	dprop->remove = remove;
	dprop->clear = clear;
	dprop->prop = prop;
	dprop->relative = relative;
	eina_hash_add(edesc->properties, name, dprop);
	DBG("Property %s (%d) added to %s", name, prop->type, edesc->name);
}

void ender_descriptor_init(void)
{
	_descriptors = eina_hash_string_superfast_new(NULL);
}

void ender_descriptor_shutdown(void)
{
	eina_hash_free(_descriptors);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Get the enders registered on the library
 * @param cb
 * @param data
 */
EAPI void ender_descriptor_list(Ender_List_Callback cb, void *data)
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
EAPI Ender_Descriptor * ender_descriptor_find(const char *name)
{
	Ender_Descriptor *ed;

	ed = eina_hash_find(_descriptors, name);
	return ed;
}

/**
 * Check if an ender with name @name exists on the library
 * @param name
 */
EAPI Eina_Bool ender_descriptor_exists(const char *name)
{
	Ender_Descriptor *desc;

	desc = eina_hash_find(_descriptors, name);
	if (desc) return EINA_TRUE;
	else return EINA_FALSE;
}

/**
 *
 */
EAPI void ender_descriptor_property_list(Ender_Descriptor *ed, Ender_Property_List_Callback cb, void *data)
{
	Eina_Iterator *it;
	char *name;

	it = eina_hash_iterator_key_new(ed->properties);
	while (eina_iterator_next(it, (void **)&name))
	{
		cb(ed, name, data);
	}
	eina_iterator_free(it);
}

/**
 *
 */
EAPI Ender_Type ender_descriptor_type(Ender_Descriptor *ed)
{
	return ed->type;
}

/**
 *
 */
EAPI const char * ender_descriptor_name_get(Ender_Descriptor *edesc)
{
	return edesc->name;
}

/**
 *
 */
EAPI Ender_Descriptor * ender_descriptor_parent(Ender_Descriptor *edesc)
{
	return edesc->parent;
}

/**
 *
 */
EAPI Ender_Property * ender_descriptor_property_get(Ender_Descriptor *ed, const char *name)
{
	return _property_get(ed, name);
}
