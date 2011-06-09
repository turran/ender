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
Ender_Descriptor * ender_descriptor_new(const char *name, Ender_Creator creator,
		Ender_Descriptor *parent, Ender_Type type)
{
	Ender_Descriptor *desc;

	desc = calloc(1, sizeof(Ender_Descriptor));
	desc->name = strdup(name);
	desc->parent = parent;
	desc->create = creator;
	desc->type = type;
	desc->properties = eina_hash_string_superfast_new(NULL);

	eina_hash_add(_descriptors, name, desc);

	return desc;
}


void ender_descriptor_property_add(Ender_Descriptor *edesc, const char *name,
	Ender_Container *prop, Ender_Getter get, Ender_Setter set,
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
	/* TODO remove every descriptor */
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

EAPI Ender_Descriptor * ender_descriptor_find_with_namespace(const char *name, const char *ns_name)
{
	Ender_Namespace *ns;
	Ender_Descriptor *ed;

	ns = ender_namespace_find(ns_name);
	if (!ns) return NULL;
	return ender_namespace_descriptor_find(ns, name);
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
EAPI Ender_Type ender_descriptor_type(Ender_Descriptor *ed)
{
	if (!ed) return 0;
	return ed->type;
}

/**
 *
 */
EAPI const char * ender_descriptor_name_get(Ender_Descriptor *edesc)
{
	if (!edesc) return NULL;

	return edesc->name;
}

/**
 *
 */
EAPI Ender_Descriptor * ender_descriptor_parent(Ender_Descriptor *edesc)
{
	if (!edesc) return NULL;

	return edesc->parent;
}

/**
 *
 */
EAPI void ender_descriptor_property_list(Ender_Descriptor *ed, Ender_Property_List_Callback cb, void *data)
{
	Ender_Property *prop;
	Eina_Iterator *it;

	if (!ed || !cb) return;

	it = eina_hash_iterator_data_new(ed->properties);
	while (eina_iterator_next(it, (void **)&prop))
	{
		cb(prop, data);
	}
	eina_iterator_free(it);
}

/**
 *
 */
EAPI Ender_Property * ender_descriptor_property_get(Ender_Descriptor *ed, const char *name)
{
	if (!ed) return NULL;
	return _property_get(ed, name);
}
