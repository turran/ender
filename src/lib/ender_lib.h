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

#ifndef _ENDER_LIB_H_
#define _ENDER_LIB_H_

/**
 * @defgroup Ender_Lib_Group Lib
 * @brief A library matches the C library you want to describe
 * @{
 */

EAPI const Ender_Lib * ender_lib_find(const char *name);

EAPI int ender_lib_version_get(const Ender_Lib *thiz);
EAPI const char * ender_lib_name_get(const Ender_Lib *thiz);
EAPI Eina_List * ender_lib_dependencies_get(const Ender_Lib *thiz);
EAPI Ender_Item * ender_lib_item_find(const Ender_Lib *thiz, const char *name);
EAPI Eina_List * ender_lib_item_list(const Ender_Lib *thiz, Ender_Item_Type type);

/**
 * @}
 */

#endif
