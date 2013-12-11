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

#ifndef _ENDER_PROPERTY_H
#define _ENDER_PROPERTY_H

/**
 * @defgroup Ender_Property_Group Property
 * @{
 */

typedef struct _Ender_Property Ender_Property;

typedef enum _Ender_Property_Flag
{
	ENDER_GET = (1 << 0),
	ENDER_SET = (1 << 1),
	ENDER_ADD = (1 << 2),
	ENDER_REMOVE = (1 << 3),
	ENDER_CLEAR = (1 << 4),
	ENDER_IS_SET = (1 << 5),
} Ender_Property_Flag;


EAPI Ender_Value_Type ender_property_type_get(Ender_Property *p);
EAPI Ender_Container * ender_property_container_get(Ender_Property *p);
EAPI Eina_Bool ender_property_is_relative(Ender_Property *p);
EAPI const char * ender_property_name_get(Ender_Property *p);
EAPI Ender_Property_Flag ender_property_flags_get(Ender_Property *p);

/**
 * @}
 */

