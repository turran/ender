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
typedef struct _Ender_Descriptor_Property
{
	Ender_Getter get;
	Ender_Setter set;
	Ender_Add add;
	Ender_Remove remove;
	Ender_Clear clear;
} Ender_Descriptor_Property;

typedef void (*Ender_Value_Accessor)(Ender_Value *v, Ender_Accessor acc,
		Enesim_Renderer *e);
typedef void (*Ender_Value_Relative_Accessor)(Ender_Value *v, Ender_Accessor acc,
		Ender_Element *e, Enesim_Renderer *parent);


static Ender_Value_Accessor _setters[ENDER_PROPERTY_TYPES];
static Ender_Value_Accessor _getters[ENDER_PROPERTY_TYPES];
static Ender_Value_Relative_Accessor _relative_accessors[ENDER_PROPERTY_TYPES];
static Eina_Hash *_descriptors = NULL;
static Ender_Property * _descriptor_property_get(Ender_Descriptor *e, const char *name);

static Ender_Property * _descriptor_property_get(Ender_Descriptor *e, const char *name)
{
	Ender_Property *prop;

	prop = eina_hash_find(e->properties, name);
	if (prop) return prop;
	if (!e->parent) return NULL;

	return _descriptor_property_get(e->parent, name);
}

static inline void _value_set(Ender_Property *p, Ender_Element *e, Ender_Value *v,
		Ender_Accessor acc)
{
	if (ender_property_is_relative(p))
	{
		Ender_Element *parent;

		parent = ender_element_parent_get(e);
		if (parent)
		{
			Enesim_Renderer *rparent;

			rparent = ender_element_renderer_get(parent);
			_relative_accessors[v->container->type](v, acc, e, rparent);
		}
		else WRN("Trying to set a relative property with no parent");
	}
	else
	{
		Enesim_Renderer *renderer;

		renderer = ender_element_renderer_get(e);
		_setters[v->container->type](v, acc, renderer);
	}

}
/*----------------------------------------------------------------------------*
 *                     uint32 / in32 / argb / bool                            *
 *----------------------------------------------------------------------------*/
static void _ender_int32_get(Ender_Value *v, Ender_Getter get, Enesim_Renderer *e)
{
	get(e, &v->data.i32);
}

static void _ender_int32_set(Ender_Value *v, Ender_Setter set, Enesim_Renderer *e)
{
	set(e, v->data.i32);
}

static void _ender_relative_int32_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, Enesim_Renderer *parent)
{
	set(parent, e, v->data.i32);
}
/*----------------------------------------------------------------------------*
 *                                   double                                   *
 *----------------------------------------------------------------------------*/
static void _ender_double_get(Ender_Value *v, Ender_Setter set, Enesim_Renderer *e)
{
	set(e, &v->data.d);
}

static void _ender_double_set(Ender_Value *v, Ender_Setter set, Enesim_Renderer *e)
{
	set(e, v->data.d);
}

static void _ender_relative_double_set(Ender_Value *v, Ender_Setter set, Ender_Element *e,
		Enesim_Renderer *parent)
{
	set(parent, e, v->data.d);
}
/*----------------------------------------------------------------------------*
 *                                  pointer                                   *
 *----------------------------------------------------------------------------*/
/* used for string, surface, renderer, ender */
static void _ender_pointer_get(Ender_Value *v, Ender_Getter get,
		Enesim_Renderer *e)
{
	get(e, &v->data.ptr);
}

/* used for string, surface, struct, matrix, renderer */
static void _ender_pointer_set(Ender_Value *v, Ender_Setter set,
		Enesim_Renderer *e)
{
	set(e, v->data.ptr);
}

/* used for string, surface, struct, matrix, renderer */
static void _ender_relative_pointer_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, Enesim_Renderer *parent)
{
	set(parent, e, v->data.ptr);
}
/*----------------------------------------------------------------------------*
 *                                   ender                                    *
 *----------------------------------------------------------------------------*/
static void _ender_ender_set(Ender_Value *v, Ender_Setter set,
		Enesim_Renderer *e)
{
	Ender_Element *child;

	child = v->data.ptr;
	set(e, child);
	ender_element_parent_set(child, ender_element_renderer_from(e));
}

static void _ender_relative_ender_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, Enesim_Renderer *parent)
{
	set(parent, e, v->data.ptr);
}

/*----------------------------------------------------------------------------*
 *                        The property interface                              *
 *----------------------------------------------------------------------------*/
/* this one is used for set/add/remove */

static void _property_set(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Descriptor_Property *dprop = data;

	_value_set(p, e, v, dprop->set);
}

static void _property_add(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Descriptor_Property *dprop = data;

	_value_set(p, e, v, dprop->add);
}

static void _property_remove(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Descriptor_Property *dprop = data;

	_value_set(p, e, v, dprop->remove);
}

static void _property_get(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Descriptor_Property *dprop = data;
	Enesim_Renderer *renderer;

	renderer = ender_element_renderer_get(e);
	_getters[v->container->type](v, dprop->get, renderer);
}

static void _property_clear(Ender_Property *p, Ender_Element *e, void *data)
{
	Ender_Descriptor_Property *dprop = data;
	Enesim_Renderer *renderer;

	renderer = ender_element_renderer_get(e);
	dprop->clear(renderer);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Descriptor * ender_descriptor_new(const char *name, Ender_Namespace *ns,
		Ender_Creator creator,
		Ender_Descriptor *parent, Ender_Type type)
{
	Ender_Descriptor *desc;

	desc = calloc(1, sizeof(Ender_Descriptor));
	desc->name = strdup(name);
	desc->parent = parent;
	desc->create = creator;
	desc->type = type;
	desc->ns = ns;
	desc->properties = eina_hash_string_superfast_new(NULL);

	eina_hash_add(_descriptors, name, desc);

	return desc;
}

Ender_Property * ender_descriptor_property_add(Ender_Descriptor *edesc, const char *name,
		Ender_Container *ec, Ender_Getter get, Ender_Setter set,
		Ender_Add add, Ender_Remove remove, Ender_Clear clear,
		Eina_Bool relative)
{
	Ender_Property *prop;
	Ender_Descriptor_Property *dprop;

	prop = eina_hash_find(edesc->properties, name);
	if (prop)
	{
		WRN("Property %s already found on %s. Not adding it", name, edesc->name);
		return NULL;
	}
	dprop = calloc(1, sizeof(Ender_Descriptor_Property));
	dprop->get = get;
	dprop->set = set;
	dprop->add = add;
	dprop->remove = remove;
	dprop->clear = clear;

	prop = ender_property_new(name, ec,
			get ? _property_get : NULL,
			set ? _property_set : NULL,
			add ? _property_add : NULL,
			remove ? _property_remove : NULL,
			clear ? _property_clear : NULL,
			relative, dprop);
	eina_hash_add(edesc->properties, name, prop);
	DBG("Property %s added to %s", name, edesc->name);

	return prop;
}

void ender_descriptor_init(void)
{
	_descriptors = eina_hash_string_superfast_new(NULL);
	/* setters */
	_setters[ENDER_BOOL] = _ender_int32_set;
	_setters[ENDER_UINT32] = _ender_int32_set;
	_setters[ENDER_INT32] = _ender_int32_set;
	_setters[ENDER_DOUBLE] = _ender_double_set;
	_setters[ENDER_ARGB] = _ender_int32_set;
	_setters[ENDER_COLOR] = _ender_int32_set;
	_setters[ENDER_STRING] = _ender_pointer_set;
	_setters[ENDER_MATRIX] = _ender_pointer_set;
	_setters[ENDER_RENDERER] = _ender_pointer_set;
	_setters[ENDER_SURFACE] = _ender_pointer_set;
	_setters[ENDER_ENDER] = _ender_ender_set;
	_setters[ENDER_LIST] = _ender_pointer_set;
	_setters[ENDER_STRUCT] = _ender_pointer_set;
	/* getters */
	_getters[ENDER_BOOL] = _ender_int32_get;
	_getters[ENDER_UINT32] = _ender_int32_get;
	_getters[ENDER_INT32] = _ender_int32_get;
	_getters[ENDER_DOUBLE] = _ender_double_get;
	_getters[ENDER_ARGB] = _ender_int32_get;
	_getters[ENDER_COLOR] = _ender_int32_get;
	_getters[ENDER_STRING] = _ender_pointer_get;
	/* TODO handle the matrix case correctly */
	_getters[ENDER_MATRIX] = _ender_pointer_get;
	_getters[ENDER_RENDERER] = _ender_pointer_get;
	_getters[ENDER_SURFACE] = _ender_pointer_get;
	_getters[ENDER_ENDER] = _ender_pointer_get;
	_getters[ENDER_LIST] = _ender_pointer_get;
	_getters[ENDER_STRUCT] = _ender_pointer_get;
	/* relative setters */
	_relative_accessors[ENDER_BOOL] = _ender_relative_int32_set;
	_relative_accessors[ENDER_UINT32] = _ender_relative_int32_set;
	_relative_accessors[ENDER_INT32] = _ender_relative_int32_set;
	_relative_accessors[ENDER_DOUBLE] = _ender_relative_double_set;
	_relative_accessors[ENDER_ARGB] = _ender_relative_int32_set;
	_relative_accessors[ENDER_COLOR] = _ender_relative_int32_set;
	_relative_accessors[ENDER_STRING] = _ender_relative_pointer_set;
	_relative_accessors[ENDER_MATRIX] = _ender_relative_pointer_set;
	_relative_accessors[ENDER_RENDERER] = _ender_relative_pointer_set;
	_relative_accessors[ENDER_SURFACE] = _ender_relative_pointer_set;
	_relative_accessors[ENDER_ENDER] = _ender_relative_ender_set;
	_relative_accessors[ENDER_LIST] = _ender_relative_pointer_set;
	_relative_accessors[ENDER_STRUCT] = _ender_relative_pointer_set;
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
	return _descriptor_property_get(ed, name);
}

/**
 *
 */
EAPI Ender_Namespace * ender_descriptor_namespace_get(Ender_Descriptor *ed)
{
	if (!ed) return NULL;
	return ed->ns;
}
