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

/* TODO */
typedef void (*Ender_Namespace_Init)(void);

static Eina_Hash *_namespaces = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_namespace_init(void)
{
	_namespaces = eina_hash_string_superfast_new(NULL);
}

void ender_namespace_shutdown(void)
{
	/* TODO remove every namespace */
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
		Ender_Type type;

		type = ender_descriptor_type(descriptor);
		printf("\t %s \"%s\" {\n", ender_type_name_get(type), ender_descriptor_name_get(descriptor));
		printf("\t};\n");
	}
	printf("};\n");
	eina_iterator_free(it);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Namespace * ender_namespace_new(const char *name)
{
	Ender_Namespace *namespace;

	if (!name) return NULL;

	/* check if we already have the namespace */
	namespace = eina_hash_find(_namespaces, name);
	if (!namespace)
	{
		namespace = malloc(sizeof(Ender_Namespace));
		namespace->name = strdup(name);
		namespace->descriptors = eina_hash_string_superfast_new(NULL);
		eina_hash_add(_namespaces, name, namespace);
	}
	return namespace;
}

EAPI Ender_Namespace * ender_namespace_find(const char *name)
{
	Ender_Namespace *namespace;

	if (!name) return NULL;

	namespace = eina_hash_find(_namespaces, name);
	return namespace;
}

EAPI Ender_Descriptor * ender_namespace_descriptor_find(Ender_Namespace *ns, const char *name)
{
	if (!ns || !name) return NULL;

	return eina_hash_find(ns->descriptors, name);
}

EAPI Ender_Descriptor * ender_namespace_descriptor_add(Ender_Namespace *ens, const char *name, Ender_Creator creator, Ender_Descriptor *parent, Ender_Type type)
{
	Ender_Descriptor *desc;

	if (!name || !ens) return NULL;
	desc = ender_descriptor_new(name, creator, parent, type);
	if (!desc) return NULL;
	DBG("class %s@%s registered correctly %p", name, ens->name, desc);
	eina_hash_add(ens->descriptors, name, desc);

	return desc;
}

EAPI const char * ender_namespace_name_get(Ender_Namespace *ns)
{
	if (!ns) return NULL;
	return ns->name;
}
