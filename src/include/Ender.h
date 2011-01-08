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
 *   [ABSTRACT | CLASS] NAME {
 *     [float | color | renderer | unsigned int] NAME;
 *   }
 * }
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
typedef void (*Ender_List_Callback)(const char *name, void *data);
typedef void (*Ender_Property_List_Callback)(Ender *e, const char *name, void *data);

typedef enum _Ender_Property_Type
{
	ENDER_UINT32,
	ENDER_INT32,
	ENDER_FLOAT,
	ENDER_DOUBLE,
	ENDER_COLOR,
	ENDER_STRING,
	ENDER_MATRIX,
	ENDER_RENDERER,
	ENDER_LIST,
	ENDER_SURFACE,
	ENDER_VALUES,
} Ender_Property_Type;


EAPI void ender_init(void);
EAPI void ender_shutdown(void);
EAPI void ender_list(Ender_List_Callback cb, void *data);

EAPI Eina_Bool ender_exists(const char *name);
EAPI Ender * ender_new(const char *name);
EAPI void ender_delete(Ender *e);
EAPI const char * ender_name_get(Ender *e);

EAPI Ender_Property * ender_property_get(Ender *e, const char *name);
EAPI void ender_property_list(Ender *e, Ender_Property_List_Callback cb, void *data);
EAPI Ender_Property_Type ender_property_type(Ender_Property *p);
EAPI const Eina_Array * ender_property_sub(Ender_Property *p);

EAPI void ender_value_get(Ender *e, ...);
EAPI void ender_value_set(Ender *e, ...);
EAPI Enesim_Renderer * ender_renderer_get(Ender *e);

/**
 * @}
 */

#endif /*_ENDER_H*/
