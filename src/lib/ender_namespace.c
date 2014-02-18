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
	Eina_Hash *descriptors;
};

static Eina_Hash *_namespaces = NULL;

static void _ender_namespace_free(void *data)
{
	Ender_Namespace *thiz = data;

	if (thiz->name)
		free(thiz->name);
	eina_hash_free(thiz->descriptors);
	free(thiz);
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

const Ender_Instance_Descriptor * ender_namespace_instance_find(
		const Ender_Namespace *thiz, const char *name)
{
	if (!thiz) return NULL;
	DBG("Looking for instance '%s' on namespace '%s'", name, thiz->name);
	return eina_hash_find(thiz->descriptors, name); 
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Namespace * ender_namespace_register(const char *name)
{
	Ender_Namespace *thiz;

	DBG("Registering namespace '%s'", name);
	if (!name) return NULL;

	thiz = eina_hash_find(_namespaces, name);
	if (thiz) return thiz;

	thiz = calloc(1, sizeof(Ender_Namespace));
	thiz->name = strdup(name);
	thiz->descriptors = eina_hash_string_superfast_new(EINA_FREE_CB(free));

	/* add it */
	eina_hash_add(_namespaces, name, thiz);

	return thiz;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_namespace_unregister(Ender_Namespace *thiz)
{
	if (!thiz) return;
	DBG("Unregistering namespace '%s'", thiz->name);
	eina_hash_del(_namespaces, thiz->name, thiz);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const Ender_Namespace * ender_namespace_find(const char *name)
{
	DBG("Looking for namespace '%s'", name);
	if (!name) return NULL;
	return eina_hash_find(_namespaces, name);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_namespace_instance_register(Ender_Namespace *thiz,
		const Ender_Instance_Descriptor *descriptor, const char *name)
{
	Ender_Instance_Descriptor *d;

	if (!thiz) return EINA_FALSE;
	if (!descriptor) return EINA_FALSE;
	if (!name) return EINA_FALSE;

	DBG("Registering instance '%s' on namespace '%s'", name, thiz->name);
	d = eina_hash_find(thiz->descriptors, name);
	if (d) return EINA_FALSE;

	d = calloc(1, sizeof(Ender_Instance_Descriptor));
	*d = *descriptor;

	eina_hash_add(thiz->descriptors, name, d);
	return EINA_TRUE;
}

#if 0
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
EAPI Ender_Descriptor * ender_namespace_descriptor_find(Ender_Namespace *thiz, const char *name)
{
	Ender_Descriptor *ret;
	if (!thiz || !name) return NULL;

	ret = eina_hash_find(thiz->descriptors, name);
	if (!ret)
	{
		/* TODO look for its "using"? */
	}
	return ret;
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
#endif
