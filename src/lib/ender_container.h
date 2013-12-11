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
#ifndef _ENDER_CONTAINER_H
#define _ENDER_CONTAINER_H

/**
 * @defgroup Ender_Container_Group Container
 * @{
 */

typedef struct _Ender_Container Ender_Container;

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

#endif
