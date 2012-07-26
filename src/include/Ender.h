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
#include <Enesim.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ENDER_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_ENDER_BUILD */
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
typedef struct _Ender_Container Ender_Container;
typedef struct _Ender_Descriptor Ender_Descriptor;
typedef struct _Ender_Namespace Ender_Namespace;
typedef struct _Ender_Constraint Ender_Constraint;

typedef void (*Ender_Event_Callback)(Ender_Element *e, const char *event_name, void *event_data, void *data);
typedef void (*Ender_New_Callback)(Ender_Element *e, void *data);

typedef enum _Ender_Descriptor_Type
{
	ENDER_ABSTRACT,
	ENDER_CLASS,
	ENDER_TYPES,
} Ender_Descriptor_Type;

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
	ENDER_MATRIX,
	ENDER_OBJECT,
	ENDER_SURFACE,
	ENDER_ENDER,
	ENDER_POINTER,
	ENDER_VALUE,
	/* compound types */
	ENDER_LIST,
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
EAPI void ender_init(int *argc, char ***argv);
EAPI void ender_shutdown(void);

/**
 * @}
 * @defgroup Ender_Container_Group Container
 * @{
 */
EAPI Ender_Container * ender_container_new(Ender_Value_Type t);
EAPI Ender_Container * ender_container_ref(Ender_Container *thiz);
EAPI Ender_Container * ender_container_unref(Ender_Container *thiz);

EAPI void ender_container_register(Ender_Container *thiz, const char *name);
EAPI Ender_Container * ender_container_find(const char *name);

EAPI Eina_Bool ender_container_is_compound(Ender_Container *ec);
EAPI Ender_Container * ender_container_compound_get(Ender_Container *ec, unsigned int idx);
EAPI Ender_Container * ender_container_compound_get_by_name(Ender_Container *ec, const char *name, unsigned int *idx);
EAPI void ender_container_compound_get_extended(Ender_Container *ec, unsigned int idx, Ender_Container **c, const char **name);
EAPI size_t ender_container_size_get(Ender_Container *ec);
EAPI size_t ender_container_compound_size_get(Ender_Container *ec);
EAPI unsigned int ender_container_compound_count(Ender_Container *ec);
EAPI void ender_container_add(Ender_Container *ec, const char *name, Ender_Container *sub);
EAPI Ender_Value_Type ender_container_type_get(Ender_Container *c);
EAPI const char * ender_container_registered_name_get(Ender_Container *thiz);

/**
 * @}
 * @defgroup Ender_Value_Group Value
 * @{
 */

typedef struct _Ender_Value Ender_Value;
typedef void (*Ender_Value_Free)(Ender_Value *value, void *data);

EAPI Ender_Value * ender_value_basic_new(Ender_Value_Type type);
EAPI Ender_Value * ender_value_list_new(Ender_Value_Type child_type);
EAPI Ender_Value * ender_value_new_container_from(Ender_Container *container);
Ender_Value * ender_value_new_container_static_from(Ender_Container *ec);
EAPI Ender_Container * ender_value_container_get(Ender_Value *value);
EAPI Ender_Value_Type ender_value_type_get(Ender_Value *value);

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

EAPI void ender_value_argb_set(Ender_Value *value, Enesim_Argb argb);
EAPI Enesim_Argb ender_value_argb_get(const Ender_Value *value);

EAPI void ender_value_color_set(Ender_Value *value, Enesim_Color argb);
EAPI Enesim_Color ender_value_color_get(const Ender_Value *value);

EAPI void ender_value_matrix_set(Ender_Value *value, Enesim_Matrix *matrix);
EAPI void ender_value_static_matrix_set(Ender_Value *value, const Enesim_Matrix *matrix);
EAPI Enesim_Matrix * ender_value_matrix_get(const Ender_Value *value);

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

EAPI void ender_value_surface_set(Ender_Value *value, Enesim_Surface *surface);
EAPI Enesim_Surface * ender_value_surface_get(const Ender_Value *value);

EAPI void ender_value_pointer_set(Ender_Value *value, void *ptr, Ender_Value_Free free_cb, void *user_data);
EAPI void * ender_value_pointer_get(const Ender_Value *value);

EAPI void ender_value_list_add(Ender_Value *value, Ender_Value *child);
EAPI void ender_value_list_set(Ender_Value *value, Eina_List *list);
EAPI const Eina_List * ender_value_list_get(const Ender_Value *value);

EAPI Ender_Value * ender_value_ref(Ender_Value *thiz);
EAPI void ender_value_unref(Ender_Value *thiz);

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
		const char *name,
		Ender_Creator creator, Ender_Destructor destructor,
		Ender_Descriptor *parent, Ender_Descriptor_Type type);
EAPI const char * ender_namespace_name_get(Ender_Namespace *thiz);
EAPI int ender_namespace_version_get(Ender_Namespace *thiz);

/**
 * @}
 * @defgroup Ender_Descriptor_Group Descriptor
 * @{
 */
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

typedef void (*Ender_Property_List_Callback)(Ender_Property *prop, void *data);

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
		Eina_Bool relative);
EAPI void ender_descriptor_property_list(Ender_Descriptor *ed, Ender_Property_List_Callback cb, void *data);
EAPI void ender_descriptor_property_list_recursive(Ender_Descriptor *thiz, Ender_Property_List_Callback cb, void *data);
EAPI Ender_Property * ender_descriptor_property_get(Ender_Descriptor *ed, const char *name);
EAPI void ender_descriptor_list(Ender_Descriptor_List_Callback cb, void *data);
EAPI Eina_Bool ender_descriptor_exists(const char *name);
EAPI Ender_Descriptor_Type ender_descriptor_type(Ender_Descriptor *ed);
EAPI const char * ender_descriptor_name_get(Ender_Descriptor *ed);
EAPI Ender_Descriptor * ender_descriptor_parent(Ender_Descriptor *ed);
EAPI Ender_Namespace * ender_descriptor_namespace_get(Ender_Descriptor *ed);
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

EAPI Ender_Element * ender_element_new(const char *name);
EAPI Ender_Element * ender_element_new_with_namespace(const char *name, const char *ns_name, int version);
EAPI Ender_Element * ender_element_new_namespace_from(const char *name, Ender_Namespace *ns);
EAPI Ender_Element * ender_element_new_descriptor_from(const char *name, Ender_Descriptor *desc);
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

EAPI void ender_element_new_listener_add(Ender_New_Callback cb, void *data);
EAPI void ender_element_new_listener_remove(Ender_New_Callback cb, void *data);

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
} Ender_Constraint_Type;

EAPI Ender_Constraint_Type ender_constraint_type_get(Ender_Constraint *thiz);
EAPI Ender_Constraint * ender_constraint_range_new(Ender_Value_Type type);
EAPI Ender_Constraint * ender_constraint_enum_new(void);
EAPI void ender_constraint_enum_append(Ender_Constraint *thiz, const char *name, int value);

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

EAPI void ender_event_listener_add(Ender_Element *e, const char *event_name, Ender_Event_Callback cb, void *data);
EAPI void ender_event_listener_remove(Ender_Element *e, const char *event_name, Ender_Event_Callback cb);
EAPI void ender_event_dispatch(Ender_Element *e, const char *event_name, void *event_data);

/**
 * @}
 * @defgroup Ender_Helper_Group Helper
 * @{
 */
EAPI const char * ender_value_type_string_to(Ender_Value_Type type);
EAPI const char * ender_descriptor_type_string_to(Ender_Descriptor_Type type);

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

typedef void (*Ender_Parser_On_Using)(void *data, const char *name);
typedef void (*Ender_Parser_On_Namespace)(void *data, const char *name, int version);
typedef void (*Ender_Parser_On_Object)(void *data, const char *name, Ender_Descriptor_Type type, const char *parent);
typedef void (*Ender_Parser_On_Property)(void *data, const char *name, Eina_Bool relative, Ender_Container *container); 
typedef void (*Ender_Parser_On_Container)(void *data, const char *name, Ender_Container *container); 

typedef struct _Ender_Parser_Descriptor
{
	Ender_Parser_On_Using on_using;
	Ender_Parser_On_Namespace on_namespace;
	Ender_Parser_On_Object on_object;
	Ender_Parser_On_Property on_property;
	Ender_Parser_On_Container on_container;
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

/**
 * @}
 */

#endif /*_ENDER_H*/
