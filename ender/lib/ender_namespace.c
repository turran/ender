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
struct _Ender_Namespace
{
	char *name;
	int version;
	Eina_Bool initialized;
	Ender_Namespace_Initialize initialize;
	void *initialize_data;
	Eina_Hash *descriptors;
	Eina_List *new_callbacks;
};

typedef struct _Ender_New_Listener
{
	Ender_New_Callback callback;
	void *data;
} Ender_New_Listener;

/* on this hash we store the list of namespaces, we use a list because we need
 * to support multiple versions for the same namespace name. The list is ordered
 * from the minimum version to the latest version
 */
static Eina_Hash *_namespaces = NULL;

static void _ender_namespace_free(void *data)
{
	Eina_List *namespaces = data;
	Eina_List *ll;
	Ender_Namespace *thiz;

	EINA_LIST_FOREACH(namespaces, ll, thiz)
	{
		if (thiz->name)
			free(thiz->name);
		eina_hash_free(thiz->descriptors);
		free(thiz);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_namespace_init(void)
{
	_namespaces = eina_hash_string_superfast_new(_ender_namespace_free);
}

void ender_namespace_shutdown(void)
{
	eina_hash_free(_namespaces);
}

void ender_namespace_dump(Ender_Namespace *ns)
{
	Eina_Iterator *it;
	Ender_Descriptor *descriptor;

	it = eina_hash_iterator_data_new(ns->descriptors);
	printf("namespace \"%s\" {\n", ns->name);

	while (eina_iterator_next(it, (void **)&descriptor))
	{
		Ender_Descriptor_Type type;

		type = ender_descriptor_type(descriptor);
		printf("\t %s \"%s\" {\n", ender_descriptor_type_string_to(type), ender_descriptor_name_get(descriptor));
		printf("\t};\n");
	}
	printf("};\n");
	eina_iterator_free(it);
}

Ender_Element * ender_namespace_element_new_from_descriptor(Ender_Namespace *thiz, Ender_Descriptor *desc)
{
	Ender_Element *element;
	Ender_New_Listener *listener;
	Eina_List *l;

	/* TODO check that we have been initialized */
	if (!thiz->initialized)
	{
		if (thiz->initialize)
			thiz->initialize(thiz, thiz->initialize_data);
		thiz->initialized = EINA_TRUE;
	}
	element = ender_element_new(desc);
	/* call the constructor callback */
	EINA_LIST_FOREACH(thiz->new_callbacks, l, listener)
	{
		listener->callback(element, listener->data);
	}
	return element;
}

/* FIXME maybe in the future we want this event to be added and also exported */
void ender_namespace_initialize_cb_set(Ender_Namespace *thiz, Ender_Namespace_Initialize cb, void *data)
{
	thiz->initialize = cb;
	thiz->initialize_data = data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Namespace * ender_namespace_new(const char *name, int version)
{
	Ender_Namespace *thiz;
	Eina_List *namespaces, *tmp;

	if (!name) return NULL;

	namespaces = eina_hash_find(_namespaces, name);
	/* check if we already have the namespace */
	EINA_LIST_FOREACH(namespaces, tmp, thiz)
	{
		if (thiz->version == version)
			return thiz;
		else if (thiz->version > version)
			break;
	}
	/* if not append it */
	thiz = calloc(1, sizeof(Ender_Namespace));
	thiz->name = strdup(name);
	thiz->version = version;
	thiz->descriptors = eina_hash_string_superfast_new(
			(Eina_Free_Cb)ender_descriptor_free);

	tmp = eina_list_append_relative_list(namespaces, thiz, tmp);
	/* add it */
	eina_hash_set(_namespaces, name, tmp);

	return thiz;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_namespace_element_new_listener_add(Ender_Namespace *thiz, Ender_New_Callback cb, void *data)
{
	Ender_New_Listener *listener;

	listener = calloc(1, sizeof(Ender_New_Listener));
	listener->callback = cb;
	listener->data = data;
	thiz->new_callbacks = eina_list_append(thiz->new_callbacks, listener);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_namespace_element_new_listener_remove(Ender_Namespace *thiz, Ender_New_Callback cb, void *data)
{
	Ender_New_Listener *listener;
	Eina_List *l;

	EINA_LIST_FOREACH(thiz->new_callbacks, l, listener)
	{
		if (listener->callback == cb && listener->data == data)
		{
			thiz->new_callbacks = eina_list_remove(l, listener);
			break;
		}
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_namespace_element_new(Ender_Namespace *thiz, const char *name)
{
	Ender_Descriptor *desc;
	Ender_Element *element;

	desc = ender_namespace_descriptor_find(thiz, name);
	if (!desc) return NULL;
	return ender_namespace_element_new_from_descriptor(thiz, desc);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_namespace_list(Ender_Namespace_List_Callback cb, void *data)
{
	Eina_List *namespaces;
	Eina_Iterator *it;

	it = eina_hash_iterator_data_new(_namespaces);
	while (eina_iterator_next(it, (void **)&namespaces))
	{
		Ender_Namespace *thiz;
		Eina_List *tmp;

		EINA_LIST_FOREACH(namespaces, tmp, thiz)
		{
			if (!cb(thiz, data))
				break;
		}
	}
	eina_iterator_free(it);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_namespace_list_with_name(const char *name,
		Ender_Namespace_List_Callback cb, void *data)
{
	Eina_List *namespaces;
	Eina_List *tmp;
	Ender_Namespace *thiz;

	if (!name) return EINA_FALSE;

	namespaces = eina_hash_find(_namespaces, name);
	if (!namespaces) return EINA_FALSE;

	EINA_LIST_FOREACH(namespaces, tmp, thiz)
	{
		if (!cb(thiz, data))
			break;
	}
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Namespace * ender_namespace_find(const char *name, int version)
{
	Eina_List *namespaces;
	Eina_List *tmp;
	Ender_Namespace *thiz;

	if (!name) return NULL;

	namespaces = eina_hash_find(_namespaces, name);
	EINA_LIST_FOREACH(namespaces, tmp, thiz)
	{
		/* TODO we might need in the future to not be too restrictive? */
		if (thiz->version == version)
			return thiz;
	}
	return NULL;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Descriptor * ender_namespace_descriptor_find(Ender_Namespace *thiz, const char *name)
{
	if (!thiz || !name) return NULL;

	return eina_hash_find(thiz->descriptors, name);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_namespace_descriptor_list(Ender_Namespace *thiz, Ender_Descriptor_List_Callback cb, void *data)
{
	Eina_Iterator *it;
	Ender_Descriptor *desc;

	if (!thiz) return;

	it = eina_hash_iterator_data_new(thiz->descriptors);
	while (eina_iterator_next(it, (void **)&desc))
	{
		if (!cb(desc, data))
			break;
	}
	eina_iterator_free(it);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Descriptor * ender_namespace_descriptor_add(Ender_Namespace *ens,
		const char *name, Ender_Creator creator,
		Ender_Destructor destructor, Ender_Descriptor *parent,
		Ender_Descriptor_Type type, int size)
{
	Ender_Descriptor *desc;

	if (!name || !ens) return NULL;
	desc = ender_descriptor_new(name, ens, creator, destructor, parent,
			type, size);
	if (!desc) return NULL;
	DBG("class %s@%s registered correctly %p", name, ens->name, desc);
	eina_hash_add(ens->descriptors, name, desc);

	return desc;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * ender_namespace_name_get(Ender_Namespace *thiz)
{
	if (!thiz) return NULL;
	return thiz->name;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int ender_namespace_version_get(Ender_Namespace *thiz)
{
	if (!thiz) return 0;
	return thiz->version;
}

