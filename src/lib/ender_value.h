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
#ifndef _ENDER_VALUE_H
#define _ENDER_VALUE_H

/**
 * @defgroup Ender_Value_Group Value
 * @{
 */

typedef struct _Ender_Value Ender_Value;

/* TODO add an UNKNOWN */
typedef enum _Ender_Value_Type
{
	/* basic types */
	ENDER_BOOL,
	ENDER_UINT32,
	ENDER_INT32,
	ENDER_UINT64,
	ENDER_INT64,
	ENDER_DOUBLE,
	ENDER_COLOR,
	ENDER_ARGB,
	ENDER_STRING,
	ENDER_OBJECT,
	ENDER_ENDER,
	ENDER_POINTER,
	ENDER_VALUE,
	/* compound types */
	ENDER_LIST,
	/* descriptor based types? */
	ENDER_STRUCT,
	ENDER_UNION,
	ENDER_PROPERTY_TYPES,
} Ender_Value_Type;

typedef void (*Ender_Value_Free)(Ender_Value *value, void *data);

EAPI Ender_Value * ender_value_basic_new(Ender_Value_Type type);
EAPI Ender_Value * ender_value_list_new(Ender_Container *child);
EAPI Ender_Value * ender_value_new_container_from(Ender_Container *container);
EAPI Ender_Value * ender_value_new_container_static_from(Ender_Container *ec);
EAPI Ender_Container * ender_value_container_get(const Ender_Value *value);
EAPI Ender_Value_Type ender_value_type_get(const Ender_Value *value);

EAPI void * ender_value_marshal(Ender_Value *v, unsigned int *len);

EAPI void ender_value_bool_set(Ender_Value *value, Eina_Bool boolean);
EAPI Eina_Bool ender_value_bool_get(const Ender_Value *value);

EAPI void ender_value_int32_set(Ender_Value *value, int32_t i32);
EAPI int32_t ender_value_int32_get(const Ender_Value *value);

EAPI void ender_value_uint32_set(Ender_Value *value, uint32_t u32);
EAPI uint32_t ender_value_uint32_get(const Ender_Value *value);

EAPI void ender_value_int64_set(Ender_Value *value, int64_t i64);
EAPI int64_t ender_value_int64_get(const Ender_Value *value);

EAPI void ender_value_uint64_set(Ender_Value *value, uint64_t u64);
EAPI uint64_t ender_value_uint64_get(const Ender_Value *value);

EAPI void ender_value_double_set(Ender_Value *value, double d);
EAPI double ender_value_double_get(const Ender_Value *value);

EAPI void ender_value_argb_set(Ender_Value *value, uint32_t argb);
EAPI uint32_t ender_value_argb_get(const Ender_Value *value);

EAPI void ender_value_color_set(Ender_Value *value, uint32_t argb);
EAPI uint32_t ender_value_color_get(const Ender_Value *value);

EAPI void ender_value_string_set(Ender_Value *value, char * string);
EAPI void ender_value_static_string_set(Ender_Value *value, const char * string);
EAPI const char * ender_value_string_get(const Ender_Value *value);

EAPI void ender_value_struct_set(Ender_Value *value, void * structure);
EAPI void * ender_value_struct_get(const Ender_Value *value);

EAPI void ender_value_union_set(Ender_Value *value, int type, void *un);
EAPI void * ender_value_union_get(const Ender_Value *value, int *type);

EAPI void ender_value_object_set(Ender_Value *value, void *object);
EAPI void * ender_value_object_get(const Ender_Value *value);

EAPI void ender_value_ender_set(Ender_Value *value, Ender_Element *ender);
EAPI Ender_Element * ender_value_ender_get(const Ender_Value *value);

EAPI void ender_value_pointer_set(Ender_Value *value, void *ptr, Ender_Value_Free free_cb, void *user_data);
EAPI void * ender_value_pointer_get(const Ender_Value *value);

EAPI void ender_value_list_add(Ender_Value *value, Ender_Value *child);
EAPI void ender_value_list_set(Ender_Value *value, Eina_List *list);
EAPI const Eina_List * ender_value_list_get(const Ender_Value *value);

EAPI Ender_Value * ender_value_ref(Ender_Value *thiz);
EAPI void ender_value_unref(Ender_Value *thiz);

#endif
