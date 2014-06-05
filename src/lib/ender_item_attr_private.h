/* ENDER - Enesim's descriptor library
 * Copyright (C) 2010 - 2012 Jorge Luis Zapata
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

#ifndef _ENDER_ITEM_ATTR_PRIVATE_H_
#define _ENDER_ITEM_ATTR_PRIVATE_H_

#include "ender_item_private.h"

Ender_Item * ender_item_attr_new(void);
void ender_item_attr_type_set(Ender_Item *i, Ender_Item *t);
void ender_item_attr_setter_set(Ender_Item *i, Ender_Item *f);
void ender_item_attr_getter_set(Ender_Item *i, Ender_Item *f);
void ender_item_attr_offset_set(Ender_Item *i, ssize_t offset);

#endif
