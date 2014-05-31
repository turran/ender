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
#include "ender_private.h"

#include "ender_main.h"
#include "ender_item.h"

#include "ender_main_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Ender_Item {
	Ender_Item *api;
	Ender_Item_Type type;
	char *name;
	int ref;
};

static void _ender_item_free(Ender_Item *thiz)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * @brief Increase the reference counter of an item
 * @param[in] thiz The item
 * @return The input parameter @a thiz for programming convenience
 */
EAPI Ender_Item * ender_item_ref(Ender_Item *thiz)
{
	if (!thiz) return thiz;
	thiz->ref++;
	return thiz;

}

/**
 * @brief Decrease the reference counter of an item
 * @param[in] thiz The item
 */
EAPI void ender_item_unref(Ender_Item *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
	}
}

EAPI const char * ender_item_name_get(Ender_Item *thiz)
{
	return thiz->name;
}

EAPI Ender_Item_Type ender_item_type_get(Ender_Item *thiz)
{
	return thiz->type;
}
