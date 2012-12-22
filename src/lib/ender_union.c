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
#include "Ender.h"
#include "ender_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                        The property interface                              *
 *----------------------------------------------------------------------------*/
static void _property_set(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
}

static void _property_add(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
}

static void _property_remove(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
}

static void _property_get(Ender_Property *p, Ender_Element *e, Ender_Value *v, void *data)
{
}

static void _property_clear(Ender_Property *p, Ender_Element *e, void *data)
{
}

static Eina_Bool _property_is_set(Ender_Property *p, Ender_Element *e, void *data)
{
}

static void _property_free(void *data)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Property * ender_union_property_add(Ender_Descriptor *edesc, const char *name,
		Ender_Container *ec, Ender_Getter get, Ender_Setter set,
		Ender_Add add, Ender_Remove remove, Ender_Clear clear,
		Ender_Is_Set is_set,
		Eina_Bool relative)
{
	return NULL;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/


