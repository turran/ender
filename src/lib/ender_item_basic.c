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
#include "ender_value.h"
#include "ender_item.h"
#include "ender_item_basic.h"

#include "ender_main_private.h"
#include "ender_item_basic_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENDER_ITEM_BASIC(o) (Ender_Item_Basic *)(ender_item_data_get(o))

typedef struct _Ender_Item_Basic
{
	Ender_Value_Type vtype;
} Ender_Item_Basic;

/*----------------------------------------------------------------------------*
 *                             Item descriptor                                *
 *----------------------------------------------------------------------------*/
static void _ender_item_basic_init(Ender_Item *i)
{
	i->type = ENDER_ITEM_TYPE_BASIC;
}

static void _ender_item_basic_deinit(Ender_Item *i)
{
	Ender_Item_Basic *thiz;

	thiz = ENDER_ITEM_BASIC(i);
	free(thiz);
}

static Ender_Item_Descriptor _descriptor = {
	/* .init 	= */ _ender_item_basic_init,
	/* .deinit 	= */ _ender_item_basic_deinit,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_basic_new(void)
{
	Ender_Item *i;
	Ender_Item_Basic *thiz;

	thiz = calloc(1, sizeof(Ender_Item_Basic));
	i = ender_item_new(&_descriptor, thiz);
	return i;
}

void ender_item_basic_value_type_set(Ender_Item *i, Ender_Value_Type type)
{
	Ender_Item_Basic *thiz;

	thiz = ENDER_ITEM_BASIC(i);
	thiz->vtype = type;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Get the type of the basic item
 * @param i The basic item to get the type from
 * @return The type of the basic item
 */
EAPI Ender_Value_Type ender_item_basic_value_type_get(Ender_Item *i)
{
	Ender_Item_Basic *thiz;

	thiz = ENDER_ITEM_BASIC(i);
	return thiz->vtype;
}
