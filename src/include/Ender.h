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
	ENDER_PROPERTY_TYPES,
} Ender_Property_Type;

typedef union _Ender_Value
{
	/* basic types */
	uint32_t uint32;
	int32_t int32;
	double d;
	Enesim_Color color;
	char *string;
	Enesim_Matrix matrix;
	Enesim_Renderer *renderer;
	Enesim_Surface *surface;
	Ender *ender;
	/* compound types */
	Eina_List *list;
} Ender_Value;

/*
 * @defgroup Ender_Main_Group Main
 * @{
 */
EAPI void ender_init(void);
EAPI void ender_shutdown(void);

/**
 * @}
 * @defgroup Ender_Descriptor_Group Descriptor Group
 * @{
 */
typedef struct _Ender_Descriptor Ender_Descriptor;
typedef void (*Ender_List_Callback)(const char *name, void *data);
typedef void (*Ender_Property_List_Callback)(Ender_Descriptor *e, const char *name, void *data);

EAPI void ender_descriptor_property_list(Ender_Descriptor *ed, Ender_Property_List_Callback cb, void *data);
EAPI void ender_descriptor_list(Ender_List_Callback cb, void *data);
EAPI Eina_Bool ender_descriptor_exists(const char *name);
EAPI Ender_Type ender_descriptor_type(Ender_Descriptor *ed);

/**
 * @}
 * @defgroup Ender_Element_Group Element Group
 * @{
 */
EAPI Ender * ender_element_new(const char *name);
EAPI void ender_element_delete(Ender *e);
EAPI const char * ender_element_name_get(Ender *e);

EAPI Ender_Property * ender_element_property_get(Ender *e, const char *name);
EAPI void ender_element_value_get(Ender *e, ...);
EAPI void ender_element_value_set(Ender *e, ...);
EAPI void ender_element_value_get_simple(Ender *e, const char *name, Ender_Value *value);
EAPI void ender_element_value_set_simple(Ender *e, const char *name, Ender_Value *value);

EAPI Enesim_Renderer * ender_element_renderer_get(Ender *e);

/**
 * @}
 * @defgroup Ender_Other_Group Other Group
 * @{
 */

EAPI void ender_event_listener_add(Ender *e, const char *event_name, Ender_Event_Callback cb, void *data);
EAPI void ender_event_listener_remove(Ender *e, const char *event_name, Ender_Event_Callback cb);
EAPI void ender_event_dispatch(Ender *e, const char *event_name, void *event_data);

EAPI Ender_Property_Type ender_property_type(Ender_Property *p);
EAPI const Eina_Array * ender_property_sub(Ender_Property *p);

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
