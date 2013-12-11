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

#ifndef _ENDER_MARSHALLER_H
#define _ENDER_MARSHALLER_H

/**
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
 */
