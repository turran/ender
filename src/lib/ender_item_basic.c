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
#define ENDER_ITEM_BASIC(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Ender_Item_Basic, ender_item_basic_descriptor_get())

typedef struct _Ender_Item_Basic
{
	Ender_Item base;
	Ender_Value_Type vtype;
} Ender_Item_Basic;

typedef struct _Ender_Item_Basic_Class
{
	Ender_Item_Class base;
} Ender_Item_Basic_Class;

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENDER_ITEM_DESCRIPTOR,
		Ender_Item_Basic, Ender_Item_Basic_Class,
		ender_item_basic);

static void _ender_item_basic_class_init(void *k)
{
}

static void _ender_item_basic_instance_init(void *o)
{
	Ender_Item *i;

	i = ENDER_ITEM(o);
	i->type = ENDER_ITEM_TYPE_BASIC;
}

static void _ender_item_basic_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_basic_new(void)
{
	Ender_Item *i;

	i = ENESIM_OBJECT_INSTANCE_NEW(ender_item_basic);
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
EAPI Ender_Value_Type ender_item_basic_value_type_get(Ender_Item *i)
{
	Ender_Item_Basic *thiz;

	thiz = ENDER_ITEM_BASIC(i);
	return thiz->vtype;
}