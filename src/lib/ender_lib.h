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

typedef struct _Ender_Lib Ender_Lib;

EAPI int ender_lib_version_get(const Ender_Lib *thiz);
EAPI const char * ender_lib_name_get(const Ender_Lib *thiz);
EAPI Eina_List * ender_lib_dependencies_get(const Ender_Lib *thiz);
EAPI const Ender_Lib * ender_lib_find(const char *name);

#endif
