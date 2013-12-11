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
#ifndef _ENDER_H
#define _ENDER_H

#include <Eina.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ENDER_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @mainpage Ender
 * @section intro Introduction
 * Ender is a library that loads descriptions of objects
 * them into a collection of properties that are accesible through a common API.
 *
 * @section dependencies Dependencies
 * - Eina
 */

#include "ender_main.h"
#include "ender_value.h"
#include "ender_container.h"
#include "ender_constraint.h"
#include "ender_marshaller.h"
#include "ender_function.h"
#include "ender_property.h"
#include "ender_marshaller.h"
#include "ender_namespace.h"
#include "ender_element.h"
#include "ender_descriptor.h"
#include "ender_parser.h"
#include "ender_event.h"

/**
 * @}
 * @defgroup Ender_Helper_Group Helper
 * @{
 */
EAPI const char * ender_value_type_string_to(Ender_Value_Type type);
EAPI const char * ender_descriptor_type_string_to(Ender_Descriptor_Type type);
EAPI Eina_Bool ender_descriptor_type_value_type_to(Ender_Descriptor_Type d,
		Ender_Value_Type *v);

/**
 * @}
 * @defgroup Ender_Loader_Group Loader
 * @{
 */

typedef void (*Ender_Loader_Registry_Callback)(void *data);

EAPI void ender_loader_load(const char *in);
EAPI void ender_loader_registry_callback_add(Ender_Loader_Registry_Callback cb, void *data);

/**
 * @}
 * @defgroup Eina_Ordered_Hash_Group Eina Ordered Hash
 * @{
 */

typedef struct _Eina_Ordered_Hash
{
	Eina_List *order;
	Eina_Hash *hash;
} Eina_Ordered_Hash;

EAPI Eina_Ordered_Hash * eina_ordered_hash_new(Eina_Free_Cb cb);
EAPI void eina_ordered_hash_free(Eina_Ordered_Hash *thiz);
EAPI int eina_ordered_hash_count(Eina_Ordered_Hash *thiz);
EAPI void * eina_ordered_hash_nth_get(Eina_Ordered_Hash *thiz, int nth);
EAPI void * eina_ordered_hash_find(Eina_Ordered_Hash *thiz, const char *key);
EAPI void eina_ordered_hash_add(Eina_Ordered_Hash *thiz, const char *name, void *data);
EAPI void eina_ordered_hash_del(Eina_Ordered_Hash *thiz, const char *name, void *data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /*_ENDER_H*/
