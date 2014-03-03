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
#include <Egueb_Css.h>
#include <Egueb_Dom.h>

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

EAPI void ender_init(void);
EAPI void ender_shutdown(void);

EAPI Egueb_Dom_Node * ender_document_new(void);
EAPI Egueb_Dom_Node * ender_document_instance_new(Egueb_Dom_Node *n,
		const char *id, Eina_Error *err);

EAPI Egueb_Dom_Node * ender_element_ender_new(void);
EAPI Egueb_Dom_Node * ender_element_state_new(void);
EAPI Egueb_Dom_Node * ender_element_scene_new(void);
EAPI Egueb_Dom_Node * ender_element_states_new(void);
EAPI Egueb_Dom_Node * ender_element_object_new(void);

EAPI void * ender_element_instance_object_get(Egueb_Dom_Node *n);
EAPI Eina_Bool ender_element_instance_state_set(Egueb_Dom_Node *n,
		const char *state, Eina_Error *err);

/* Shared strings */
EAPI extern Egueb_Dom_String *ENDER_ELEMENT_ENDER;
EAPI extern Egueb_Dom_String *ENDER_ELEMENT_INSTANCE;
EAPI extern Egueb_Dom_String *ENDER_ELEMENT_OBJECT;
EAPI extern Egueb_Dom_String *ENDER_ELEMENT_SCENE;
EAPI extern Egueb_Dom_String *ENDER_ELEMENT_STATES;
EAPI extern Egueb_Dom_String *ENDER_ELEMENT_STATE;

#include "ender_attr_int.h"
#include "ender_attr_double.h"
#include "ender_attr_enesim_color.h"
#include "ender_attr_font.h"
#include "ender_attr_primitive.h"
#include "ender_instance.h"
#include "ender_namespace.h"

#if 0
/**
 * @mainpage Ender
 * @section intro Introduction
 * Ender is a library that loads descriptions of objects
 * them into a collection of properties that are accesible through a common API.
 *
 * @section syntax Syntax
 * The syntax of a description file (.ender) is:
 * @code
 * namespace "[PREFIX.]NAME" {
 *   (abstract | class) "NAME" {
 *     PROPERTY "NAME";
 *   }
 * }
 * Where PROPERTY can be one of the following:
 * uint | int | double | argb | color | surface | matrix | object | ender | LIST

 * Where LIST is defined as:
 * (PROPERTY [, PROPERTY])
 *
 * @endcode
 *
 * @section examples Examples
 *
 * @section dependencies Dependencies
 * - Eina
 * - Enesim
 *
 * @todo
 */

typedef struct _Ender_Element Ender_Element;
typedef struct _Ender_Property Ender_Property;
typedef struct _Ender_Function Ender_Function;
typedef struct _Ender_Container Ender_Container;
typedef struct _Ender_Descriptor Ender_Descriptor;
typedef struct _Ender_Namespace Ender_Namespace;
typedef struct _Ender_Constraint Ender_Constraint;
typedef struct _Ender_Value Ender_Value;

typedef void (*Ender_Event_Callback)(Ender_Element *e, const char *event_name, void *event_data, void *data);
typedef void (*Ender_New_Callback)(Ender_Element *e, void *data);

typedef enum _Ender_Descriptor_Type
{
	ENDER_TYPE_ABSTRACT,
	ENDER_TYPE_CLASS,
	ENDER_TYPE_STRUCT,
	ENDER_TYPE_UNION,
	ENDER_TYPES,
} Ender_Descriptor_Type;

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

typedef enum _Ender_Property_Flag
{
	ENDER_GET = (1 << 0),
	ENDER_SET = (1 << 1),
	ENDER_ADD = (1 << 2),
	ENDER_REMOVE = (1 << 3),
	ENDER_CLEAR = (1 << 4),
	ENDER_IS_SET = (1 << 5),
} Ender_Property_Flag;

/*
 * @defgroup Ender_Main_Group Main
 * @{
 */
EAPI void ender_init(void);
EAPI void ender_shutdown(void);
EAPI void ender_version(unsigned int *major, unsigned int *minor, unsigned int *micro);

/**
 * @}
 * @defgroup Ender_Container_Group Container
 * @{
 */
EAPI Ender_Container * ender_container_new(Ender_Value_Type t);
EAPI Ender_Container * ender_container_new_descriptor_from(Ender_Descriptor *descriptor);
EAPI Ender_Container * ender_container_list_new(Ender_Container *child);
EAPI Ender_Container * ender_container_struct_new(Ender_Descriptor *descriptor);
EAPI Ender_Container * ender_container_union_new(Ender_Descriptor *descriptor);

EAPI Ender_Container * ender_container_ref(Ender_Container *thiz);
EAPI Ender_Container * ender_container_unref(Ender_Container *thiz);

EAPI Eina_Bool ender_container_is_compound(Ender_Container *ec);
EAPI Ender_Container * ender_container_compound_get(Ender_Container *ec, unsigned int idx, const char **name);
EAPI Ender_Container * ender_container_compound_get_by_name(Ender_Container *ec, const char *name, unsigned int *idx);

EAPI size_t ender_container_size_get(Ender_Container *ec);

EAPI unsigned int ender_container_compound_count(Ender_Container *ec);
EAPI Ender_Value_Type ender_container_type_get(Ender_Container *c);

EAPI void ender_container_constraint_set(Ender_Container *thiz, Ender_Constraint *c);
EAPI const Ender_Constraint * ender_container_constraint_get(Ender_Container *thiz);

EAPI Ender_Value * ender_container_value_unmarshal(Ender_Container *c, void *data, unsigned int len);

/**
 * @}
 * @defgroup Ender_Value_Group Value
 * @{
 */

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

typedef void (*Ender_Accessor)(void *o, ...);
typedef Eina_Bool (*Ender_Is_Set)(void *o);
typedef Ender_Accessor Ender_Getter;
typedef Ender_Accessor Ender_Setter;
typedef Ender_Accessor Ender_Add;
typedef Ender_Accessor Ender_Remove;
typedef void (*Ender_Clear)(void *o);

#define ENDER_IS_SET(f) ((Ender_Is_Set)(f))
#define ENDER_GETTER(f) ((Ender_Getter)(f))
#define ENDER_SETTER(f) ((Ender_Setter)(f))
#define ENDER_ADD(f) ((Ender_Add)(f))
#define ENDER_REMOVE(f) ((Ender_Remove)(f))
#define ENDER_CLEAR(f) ((Ender_Clear)(f))

/**
 * @}
 * @defgroup Ender_Namespace_Group Namespace
 * @{
 */
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
 * @defgroup Ender_Marshaller_Group Marshaller
 * @{
 */

typedef Eina_Bool (*Ender_Marshaller)(void *data, Ender_Accessor f, Ender_Value *ret,
		Eina_List *args);

EAPI Ender_Marshaller ender_marshaller_find(Ender_Container *ret,
		...);
EAPI Ender_Marshaller ender_marshaller_find_list(Ender_Container *ret,
		Eina_List *args);

EAPI Eina_Bool ender_marshaller_void__void(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args);
EAPI Eina_Bool ender_marshaller_void__string_string(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args);
EAPI Eina_Bool ender_marshaller_string__string(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args);
EAPI Eina_Bool ender_marshaller_ender__string(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args);
EAPI Eina_Bool ender_marshaller_ender__void(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args);

/**
 * @}
 * @defgroup Ender_Function_Group Function
 * @{
 */
#define ENDER_FUNCTION(f) ((Ender_Accessor)(f))

EAPI int ender_function_args_count(Ender_Function *thiz);
EAPI const Eina_List * ender_function_args_get(Ender_Function *thiz);
EAPI Ender_Container * ender_function_ret_get(Ender_Function *thiz);
EAPI const char * ender_function_name_get(Ender_Function *thiz);

/**
 * @}
 * @defgroup Ender_Descriptor_Group Descriptor
 * @{
 */

#define ENDER_DESCRIPTOR_PROPERTY_ADD_SIMPLE(d, name, vtype, prefix)		\
	ender_descriptor_property_add(d, #name, ender_container_new(vtype),	\
			prefix##_##name##_get, prefix##_##name##_set, 		\
			NULL, NULL, NULL,					\
			prefix##_##_name##_is_set,				\
			EINA_FALSE, -1)
#define ENDER_DESCRIPTOR_PROPERTY_ADD_DESCRIPTOR(d, name, pd, prefix)		\
	ender_descriptor_property_add(d, #name, 				\
			ender_container_new_descriptor_from(pd), 		\
			prefix##_##name##_get, prefix##_##name##_set, 		\
			NULL, NULL, NULL,					\
			prefix##_##_name##_is_set,				\
			EINA_FALSE, -1)

typedef void (*Ender_Property_List_Callback)(Ender_Property *prop, void *data);
typedef void (*Ender_Function_List_Callback)(Ender_Function *func, void *data);

EAPI Ender_Descriptor * ender_descriptor_find(const char *name);
EAPI Ender_Descriptor * ender_descriptor_find_with_namespace(const char *name, const char *ns, int version);
EAPI Ender_Property * ender_descriptor_property_add(Ender_Descriptor *edesc,
		const char *name,
		Ender_Container *ec,
		Ender_Getter get,
		Ender_Setter set,
		Ender_Add add,
		Ender_Remove remove,
		Ender_Clear clear,
		Ender_Is_Set is_set,
		Eina_Bool relative,
		int offset);
EAPI Ender_Property * ender_descriptor_struct_property_add(
		Ender_Descriptor *thiz,
		const char *name, Ender_Container *ec, int offset);
EAPI void ender_descriptor_property_list(Ender_Descriptor *thiz, Ender_Property_List_Callback cb, void *data);
EAPI void ender_descriptor_property_list_recursive(Ender_Descriptor *thiz, Ender_Property_List_Callback cb, void *data);
EAPI Ender_Property * ender_descriptor_property_get(Ender_Descriptor *thiz, const char *name);
EAPI Ender_Property * ender_descriptor_property_get_at(Ender_Descriptor *thiz, int idx);

EAPI Ender_Function * ender_descriptor_function_add(Ender_Descriptor *edesc, const char *name,
		Ender_Accessor f, Ender_Marshaller marshaller, Ender_Container *ret, ...);
EAPI Ender_Function * ender_descriptor_function_add_list(Ender_Descriptor *edesc, const char *name,
		Ender_Accessor f, Ender_Marshaller marshaller, Ender_Container *ret,
		Eina_List *args);
EAPI Ender_Function * ender_descriptor_function_get(Ender_Descriptor *thiz, const char *name);
EAPI void ender_descriptor_function_list(Ender_Descriptor *thiz, Ender_Function_List_Callback cb, void *data);

EAPI void ender_descriptor_list(Ender_Descriptor_List_Callback cb, void *data);
EAPI Eina_Bool ender_descriptor_exists(const char *name);
EAPI Ender_Descriptor_Type ender_descriptor_type(Ender_Descriptor *thiz);
EAPI const char * ender_descriptor_name_get(Ender_Descriptor *thiz);
EAPI size_t ender_descriptor_size_get(Ender_Descriptor *thiz);
EAPI Ender_Descriptor * ender_descriptor_parent(Ender_Descriptor *thiz);
EAPI Ender_Namespace * ender_descriptor_namespace_get(Ender_Descriptor *thiz);
EAPI Ender_Element * ender_descriptor_element_new(Ender_Descriptor *thiz);
EAPI Ender_Element * ender_descriptor_element_new_from_data(
		Ender_Descriptor *thiz, void *data);

EAPI Ender_Element * ender_descriptor_element_unmarshal(Ender_Descriptor *thiz,
		void *data, unsigned int len);
EAPI void * ender_descriptor_native_marshal(Ender_Descriptor *thiz,
		void *data, unsigned int *len);
EAPI void * ender_descriptor_native_unmarshal(Ender_Descriptor *thiz,
		const void *data, unsigned int len);
EAPI void ender_descriptor_native_unmarshal_inplace(Ender_Descriptor *thiz,
		void *native, const void *data, unsigned int len);
EAPI void * ender_descriptor_native_new(Ender_Descriptor *thiz);
EAPI void ender_descriptor_native_free(Ender_Descriptor *thiz, void *object);
EAPI void ender_descriptor_data_set(Ender_Descriptor *thiz, void *data);
EAPI void * ender_descriptor_data_get(Ender_Descriptor *thiz);

/**
 * @}
 * @defgroup Ender_Element_Group Element
 * @{
 */
typedef void (*Ender_Element_Accessor)(Ender_Element *e, Ender_Property *ep, Ender_Value *v, void *data);
typedef Eina_Bool (*Ender_Element_Is_Set)(Ender_Element *e, Ender_Property *ep, void *data);
typedef Ender_Element_Accessor Ender_Element_Getter;
typedef Ender_Element_Accessor Ender_Element_Setter;
typedef Ender_Element_Accessor Ender_Element_Add;
typedef Ender_Element_Accessor Ender_Element_Remove;
typedef void (*Ender_Element_Clear)(Ender_Element *e, Ender_Property *ep, void *data);

EAPI Ender_Element * ender_element_new_with_namespace(const char *name, const char *ns_name, int version);
EAPI Ender_Element * ender_element_ref(Ender_Element *e);
EAPI Ender_Element * ender_element_unref(Ender_Element *e);
EAPI void * ender_element_data_set(Ender_Element *e, const char *key, void *data);
EAPI void * ender_element_data_get(Ender_Element *e, const char *key);
EAPI const char * ender_element_name_get(Ender_Element *e);
EAPI Ender_Descriptor * ender_element_descriptor_get(Ender_Element *e);

EAPI Ender_Property * ender_element_property_add(Ender_Element *e, const char *name,
		Ender_Container *ec,
		Ender_Element_Getter get,
		Ender_Element_Setter set,
		Ender_Element_Add add,
		Ender_Element_Remove remove,
		Ender_Element_Clear clear,
		Ender_Element_Is_Set is_set,
		Eina_Bool relative, void *data);
EAPI Ender_Property * ender_element_property_get(Ender_Element *e, const char *name);
EAPI void ender_element_property_list(Ender_Element *e, Ender_Property_List_Callback cb, void *data);
EAPI void ender_element_property_value_set_valist(Ender_Element *e, Ender_Property *prop, va_list va_args);
EAPI void ender_element_property_value_set(Ender_Element *e, Ender_Property *prop, ...);
EAPI void ender_element_property_value_set_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value);

EAPI void ender_element_property_value_get_valist(Ender_Element *e, Ender_Property *prop, va_list va_args);
EAPI void ender_element_property_value_get(Ender_Element *e, Ender_Property *prop, ...);
EAPI void ender_element_property_value_get_simple(Ender_Element *e, Ender_Property *prop, Ender_Value **value);

EAPI void ender_element_property_value_add_valist(Ender_Element *e, Ender_Property *prop, va_list var_args);
EAPI void ender_element_property_value_add(Ender_Element *e, Ender_Property *prop, ...);
EAPI void ender_element_property_value_add_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value);

EAPI void ender_element_property_value_remove_valist(Ender_Element *e, Ender_Property *prop, va_list var_args);
EAPI void ender_element_property_value_remove(Ender_Element *e, Ender_Property *prop, ...);
EAPI void ender_element_property_value_remove_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value);

EAPI Eina_Bool ender_element_property_value_is_set(Ender_Element *e, Ender_Property *prop);

EAPI void ender_element_value_get(Ender_Element *e, const char *name, ...);
EAPI void ender_element_value_get_valist(Ender_Element *e, const char *name, va_list var_args);
EAPI void ender_element_value_get_simple(Ender_Element *e, const char *name, Ender_Value **value);

EAPI void ender_element_value_set(Ender_Element *e, const char *name, ...);
EAPI void ender_element_value_set_valist(Ender_Element *e, const char *name, va_list var_args);
EAPI void ender_element_value_set_simple(Ender_Element *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_add(Ender_Element *e, const char *name, ...);
EAPI void ender_element_value_add_valist(Ender_Element *e, const char *name, va_list var_args);
EAPI void ender_element_value_add_simple(Ender_Element *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_remove(Ender_Element *e, const char *name, ...);
EAPI void ender_element_value_remove_valist(Ender_Element *e, const char *name, va_list var_args);
EAPI void ender_element_value_remove_simple(Ender_Element *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_clear(Ender_Element *e, const char *name);

EAPI Eina_Bool ender_element_value_is_set(Ender_Element *e, const char *name);

EAPI void * ender_element_object_get(Ender_Element *e);

EAPI Ender_Element * ender_element_parent_get(Ender_Element *e);

EAPI Eina_Bool ender_element_call_valist(Ender_Element *e, const char *name, void *ret, va_list va_args);
EAPI Eina_Bool ender_element_call(Ender_Element *e, const char *name, void *ret, ...);
EAPI Eina_Bool ender_element_function_call_valist(Ender_Element *e, Ender_Function *f, void *ret, va_list va_args);
EAPI Eina_Bool ender_element_function_call(Ender_Element *e, Ender_Function *f, void *ret, ...);
EAPI Eina_Bool ender_element_function_call_simple(Ender_Element *e,
		Ender_Function *f, Ender_Value *ret, Eina_List *args);

EAPI void * ender_element_marshal(Ender_Element *e, unsigned int *len);

/**
 * @}
 * @defgroup Ender_Property_Group Property
 * @{
 */
EAPI Ender_Value_Type ender_property_type_get(Ender_Property *p);
EAPI Ender_Container * ender_property_container_get(Ender_Property *p);
EAPI Eina_Bool ender_property_is_relative(Ender_Property *p);
EAPI const char * ender_property_name_get(Ender_Property *p);
EAPI Ender_Property_Flag ender_property_flags_get(Ender_Property *p);

/**
 * @}
 * @defgroup Ender_Constraint_Group Constraint
 * @{
 */

typedef enum _Ender_Constraint_Type
{
	ENDER_CONSTRAINT_RANGE,
	ENDER_CONSTRAINT_ENUM,
	ENDER_CONSTRAINT_DESCRIPTOR,
	ENDER_CONSTRAINTS,
} Ender_Constraint_Type;

EAPI Ender_Constraint_Type ender_constraint_type_get(const Ender_Constraint *thiz);

EAPI Ender_Constraint * ender_constraint_range_new(Ender_Value_Type type);

EAPI Ender_Constraint * ender_constraint_enum_new(void);
EAPI void ender_constraint_enum_append(Ender_Constraint *thiz, const char *name, int value);

EAPI Ender_Constraint * ender_constraint_descriptor_new(
	Ender_Descriptor *descriptor);
EAPI Ender_Descriptor * ender_constraint_descriptor_descriptor_get(
	const Ender_Constraint *thiz);

/**
 * @}
 * @defgroup Ender_Other_Group Other
 * @{
 */

typedef enum _Ender_Event_Mutation_Type
{
	ENDER_EVENT_MUTATION_SET,
	ENDER_EVENT_MUTATION_ADD,
	ENDER_EVENT_MUTATION_REMOVE,
	ENDER_EVENT_MUTATION_CLEAR,
} Ender_Event_Mutation_Type;

typedef struct _Ender_Event_Mutation
{
	const char *name;
	const Ender_Value *value;
	Ender_Event_Mutation_Type type;
} Ender_Event_Mutation;

typedef struct _Ender_Event_Mutation_Property
{
	const Ender_Value *value;
	Ender_Event_Mutation_Type type;
} Ender_Event_Mutation_Property;

typedef struct _Ender_Listener Ender_Listener;

EAPI Ender_Listener * ender_event_listener_add(Ender_Element *e, const char *event_name, Ender_Event_Callback cb, void *data);
EAPI void ender_event_listener_remove(Ender_Listener *l);
EAPI void ender_event_listener_remove_full(Ender_Element *e, const char *event_name, Ender_Event_Callback cb, void *data);
EAPI void ender_event_dispatch(Ender_Element *e, const char *event_name, void *event_data);

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
 * @defgroup Ender_Parser_Group Parser
 * @{
 */

typedef struct _Ender_Parser_Definition
{
	const char *name;
	const char *alias;
} Ender_Parser_Definition;

typedef struct _Ender_Parser_Container
{
	Ender_Value_Type type;
	Eina_List *subcontainers;
	char *defined;
} Ender_Parser_Container;

typedef struct _Ender_Parser_Property
{
	Ender_Parser_Definition def;
	Ender_Parser_Container *container;
	Eina_Bool rel;
} Ender_Parser_Property;

typedef struct _Ender_Parser_Function
{
	Ender_Parser_Definition def;
	Ender_Parser_Container *ret;
	Eina_List *args;
} Ender_Parser_Function;

/* FIXME this functions will be evolving until all of them
 * are similar to the add_property
 */
typedef void (*Ender_Parser_Add_Using)(void *data, const char *name);
typedef void (*Ender_Parser_Add_Namespace)(void *data, const char *name,
		int version);
typedef void (*Ender_Parser_Add_Native)(void *data, const char *name,
		const char *alias, Ender_Descriptor_Type type, const char *parent);
typedef void (*Ender_Parser_Add_Property)(void *data, Ender_Parser_Property *p);
typedef void (*Ender_Parser_Add_Function)(void *data, Ender_Parser_Function *f);

typedef struct _Ender_Parser_Descriptor
{
	Ender_Parser_Add_Using add_using;
	Ender_Parser_Add_Namespace add_namespace;
	Ender_Parser_Add_Native add_native;
	Ender_Parser_Add_Property add_property;
	Ender_Parser_Add_Function add_function;
} Ender_Parser_Descriptor;

EAPI Eina_Bool ender_parser_parse(const char *file, Ender_Parser_Descriptor *descriptor, void *data);

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
#endif
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /*_ENDER_H*/
