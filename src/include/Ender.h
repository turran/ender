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

#include "Eina.h"
#include "Enesim.h"

/**
 * @mainpage Ender
 * @section intro Introduction
 * Ender is a library that loads descriptions of enesim renderers and abstracts
 * them into a collection of properties that are accesible through a common API.
 *
 * @section syntax Syntax
 * The syntax of a description file (.ender) is:
 * @code
 * namespace [PREFIX.]NAME {
 *   (abstract | class) NAME {
 *     PROPERTY NAME;
 *   }
 * }
 * Where PROPERTY can be one of the following:
 * uint | int | double | argb | surface | matrix | LIST

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
 * @defgroup Ender_Group API
 * @{
 *
 * @todo
 */

typedef struct _Ender Ender;
typedef struct _Ender_Property Ender_Property;
typedef struct _Ender_Container Ender_Container;

typedef void (*Ender_Event_Callback)(Ender *e, const char *event_name, void *event_data, void *data);

typedef enum _Ender_Type
{
	ENDER_ABSTRACT,
	ENDER_CLASS,
	ENDER_TYPES,
} Ender_Type;

typedef enum _Ender_Property_Type
{
	/* basic types */
	ENDER_UINT32,
	ENDER_INT32,
	ENDER_DOUBLE,
	ENDER_ARGB,
	ENDER_STRING,
	ENDER_MATRIX,
	ENDER_RENDERER,
	ENDER_SURFACE,
	ENDER_ENDER,
	/* compound types */
	ENDER_LIST,
	ENDER_STRUCT,
	ENDER_PROPERTY_TYPES,
} Ender_Property_Type;

/*
 * @defgroup Ender_Main_Group Main
 * @{
 */
EAPI void ender_init(void);
EAPI void ender_shutdown(void);

/**
 * @}
 * @defgroup Ender_Container_Group Container Group
 * @{
 */

EAPI Eina_Bool ender_container_is_compound(Ender_Container *ec);
EAPI Ender_Container * ender_container_compound_get(Ender_Container *ec, unsigned int idx);
EAPI size_t ender_container_size_get(Ender_Container *ec);
EAPI size_t ender_container_compound_size_get(Ender_Container *ec);
EAPI unsigned int ender_container_compound_count(Ender_Container *ec);
EAPI void ender_container_add(Ender_Container *ec, Ender_Container *sub);
EAPI Ender_Property_Type ender_container_type(Ender_Container *c);


/**
 * @}
 * @defgroup Ender_Value_Group Value Group
 * @{
 */

typedef struct _Ender_Value Ender_Value;

EAPI Ender_Value * ender_value_basic_new(Ender_Property_Type type);
EAPI Ender_Value * ender_value_list_new(Ender_Property_Type child_type);
EAPI Ender_Value * ender_value_new_container_from(Ender_Container *container);
EAPI Ender_Container * ender_value_container_get(Ender_Value *value);
EAPI Ender_Property_Type ender_value_type_get(Ender_Value *value);

EAPI void ender_value_int32_set(Ender_Value *value, int32_t i32);
EAPI int32_t ender_value_int32_get(Ender_Value *value);

EAPI void ender_value_uint32_set(Ender_Value *value, uint32_t u32);
EAPI uint32_t ender_value_uint32_get(Ender_Value *value);

EAPI void ender_value_double_set(Ender_Value *value, double d);
EAPI double ender_value_double_get(Ender_Value *value);

EAPI void ender_value_argb_set(Ender_Value *value, Enesim_Color argb);
EAPI Enesim_Color ender_value_argb_get(Ender_Value *value);

EAPI void ender_value_matrix_set(Ender_Value *value, Enesim_Matrix *matrix);
EAPI void ender_value_static_matrix_set(Ender_Value *value, const Enesim_Matrix *matrix);
EAPI Enesim_Matrix * ender_value_matrix_get(Ender_Value *value);

EAPI void ender_value_string_set(Ender_Value *value, char * string);
EAPI void ender_value_static_string_set(Ender_Value *value, const char * string);
EAPI char * ender_value_string_get(Ender_Value *value);

EAPI void ender_value_struct_set(Ender_Value *value, void * structure);
EAPI void ender_value_const_struct_set(Ender_Value *value, void * structure);
EAPI const void * ender_value_struct_get(Ender_Value *value);

EAPI void ender_value_renderer_set(Ender_Value *value, Enesim_Renderer *renderer);
EAPI Enesim_Renderer * ender_value_renderer_get(Ender_Value *value);

EAPI void ender_value_ender_set(Ender_Value *value, Ender *ender);
EAPI Ender * ender_value_ender_get(Ender_Value *value);

EAPI void ender_value_surface_set(Ender_Value *value, Enesim_Surface *surface);
EAPI Enesim_Surface * ender_value_surface_get(Ender_Value *value);

EAPI void ender_value_list_add(Ender_Value *value, Ender_Value *child);


EAPI void ender_value_free(Ender_Value *v);

/**
 * @}
 * @defgroup Ender_Descriptor_Group Descriptor Group
 * @{
 */
typedef struct _Ender_Descriptor Ender_Descriptor;

typedef void (*Ender_List_Callback)(const char *name, void *data);
typedef void (*Ender_Property_List_Callback)(Ender_Descriptor *e, const char *name, void *data);

EAPI Ender_Descriptor * ender_descriptor_find(const char *name);
EAPI void ender_descriptor_property_list(Ender_Descriptor *ed, Ender_Property_List_Callback cb, void *data);
EAPI Ender_Property * ender_descriptor_property_get(Ender_Descriptor *ed, const char *name);
EAPI void ender_descriptor_list(Ender_List_Callback cb, void *data);
EAPI Eina_Bool ender_descriptor_exists(const char *name);
EAPI Ender_Type ender_descriptor_type(Ender_Descriptor *ed);
EAPI const char * ender_descriptor_name_get(Ender_Descriptor *ed);
EAPI Ender_Descriptor * ender_descriptor_parent(Ender_Descriptor *ed);
/**
 * @}
 * @defgroup Ender_Element_Group Element Group
 * @{
 */
EAPI Ender * ender_element_new(const char *name);
EAPI void ender_element_delete(Ender *e);
EAPI const char * ender_element_name_get(Ender *e);
EAPI Ender_Descriptor * ender_element_descriptor_get(Ender *e);

EAPI Ender_Property * ender_element_property_get(Ender *e, const char *name);

EAPI void ender_element_value_get(Ender *e, const char *name, ...);
EAPI void ender_element_value_get_valist(Ender *e, const char *name, va_list var_args);
EAPI void ender_element_value_get_simple(Ender *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_set(Ender *e, const char *name, ...);
EAPI void ender_element_value_set_valist(Ender *e, const char *name, va_list var_args);
EAPI void ender_element_value_set_simple(Ender *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_add(Ender *e, const char *name, ...);
EAPI void ender_element_value_add_valist(Ender *e, const char *name, va_list var_args);
EAPI void ender_element_value_add_simple(Ender *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_clear(Ender *e, const char *name);

EAPI Enesim_Renderer * ender_element_renderer_get(Ender *e);

/**
 * @}
 * @defgroup Ender_Property_Group Property Group
 * @{
 */
EAPI Ender_Property_Type ender_property_type(Ender_Property *p);
EAPI Eina_Bool ender_property_is_relative(Ender_Property *p);
EAPI const char * ender_property_name_get(Ender_Property *p);

/**
 * @}
 * @defgroup Ender_Other_Group Other Group
 * @{
 */

typedef struct _Ender_Event_Mutation
{
	
} Ender_Event_Mutation;

EAPI void ender_event_listener_add(Ender *e, const char *event_name, Ender_Event_Callback cb, void *data);
EAPI void ender_event_listener_remove(Ender *e, const char *event_name, Ender_Event_Callback cb);
EAPI void ender_event_dispatch(Ender *e, const char *event_name, void *event_data);

/**
 * @}
 * @defgroup Ender_Helper_Group Helper Group
 * @{
 */
EAPI const char * ender_property_type_name_get(Ender_Property_Type type);
EAPI const char * ender_type_name_get(Ender_Type type);

/**
 * @}
 */

#endif /*_ENDER_H*/
