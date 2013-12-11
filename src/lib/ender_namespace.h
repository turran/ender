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

#ifndef _ENDER_NAMESPACE_H_
#define _ENDER_NAMESPACE_H_

/**
 * @defgroup Ender_Namespace_Group Namespace
 * @{
 */

typedef struct _Ender_Namespace Ender_Namespace;

typedef void (*Ender_New_Callback)(Ender_Element *e, void *data);
typedef Eina_Bool (*Ender_Namespace_List_Callback)(Ender_Namespace *thiz, void *data);
typedef Eina_Bool (*Ender_Descriptor_List_Callback)(Ender_Descriptor *thiz, void *data);
typedef void * (*Ender_Creator)(void);
typedef void (*Ender_Destructor)(void *);

#define ENDER_CREATOR(f) ((Ender_Creator)(f))
#define ENDER_DESTRUCTOR(f) ((Ender_Destructor)(f))

EAPI void ender_namespace_list(Ender_Namespace_List_Callback cb, void *data);
EAPI Eina_Bool ender_namespace_list_with_name(const char *name, Ender_Namespace_List_Callback cb, void *data);
EAPI Ender_Namespace * ender_namespace_new(const char *name, int version);
EAPI Ender_Namespace * ender_namespace_find(const char *name, int version);
EAPI void ender_namespace_descriptor_list(Ender_Namespace *thiz,
		Ender_Descriptor_List_Callback cb, void *data);
EAPI Ender_Descriptor * ender_namespace_descriptor_find(Ender_Namespace *thiz,
		const char *name);
EAPI Ender_Descriptor * ender_namespace_descriptor_add(Ender_Namespace *ens,
		const char *name, Ender_Creator creator,
		Ender_Destructor destructor, Ender_Descriptor *parent,
		Ender_Descriptor_Type type, int size);
EAPI const char * ender_namespace_name_get(Ender_Namespace *thiz);
EAPI int ender_namespace_version_get(Ender_Namespace *thiz);
EAPI Ender_Element * ender_namespace_element_new(Ender_Namespace *thiz, const char *name);

EAPI void ender_namespace_element_new_listener_add(Ender_Namespace *thiz, Ender_New_Callback cb, void *data);
EAPI void ender_namespace_element_new_listener_remove(Ender_Namespace *thiz, Ender_New_Callback cb, void *data);

/**
 * @}
 */

#endif
