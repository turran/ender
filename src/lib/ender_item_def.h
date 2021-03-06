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

#ifndef _ENDER_ITEM_DEF_H_
#define _ENDER_ITEM_DEF_H_

/**
 * @defgroup Ender_Def_Group Def
 * @brief A def represents a definition of an identifier based in another, i.e an alias
 * @{
 */

EAPI Ender_Item * ender_item_def_type_get(Ender_Item *i);
EAPI Eina_List * ender_item_def_functions_get(Ender_Item *i);

/**
 * @}
 */

#endif
