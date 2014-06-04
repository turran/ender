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

#ifndef _ENDER_LIB_PRIVATE_H_
#define _ENDER_LIB_PRIVATE_H_

void ender_lib_init(void);
void ender_lib_shutdown(void);
void ender_lib_register(Ender_Lib *thiz);

Ender_Lib * ender_lib_new(void);
void ender_lib_free(Ender_Lib *thiz);
void ender_lib_version_set(Ender_Lib *thiz, int version);
void ender_lib_name_set(Ender_Lib *thiz, const char *name);
void ender_lib_dependency_add(Ender_Lib *thiz, const Ender_Lib *dep);
void ender_lib_item_add(Ender_Lib *thiz, Ender_Item *i);

#endif

