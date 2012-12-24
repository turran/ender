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
typedef struct _Ender_Union
{

} Ender_Union;
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

/*----------------------------------------------------------------------------*
 *                       The descriptor interface                             *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_union_validate(const char *name, Ender_Namespace *ns,
		Ender_Creator creator,
		Ender_Destructor destructor,
		Ender_Descriptor *parent, Ender_Descriptor_Type type)
{
	if (parent)
	{
		ERR("Union '%s' can not have a parent yet", name);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static void * _ender_union_creator(Ender_Descriptor *d)
{
	return NULL;
}

static void _ender_union_destructor(Ender_Descriptor *d, void *n)
{
	free(n);
}

static Ender_Property * _ender_union_property_add(Ender_Descriptor *d,
		const char *name, Ender_Container *ec, Ender_Getter get,
		Ender_Setter set, Ender_Add add, Ender_Remove remove,
		Ender_Clear clear, Ender_Is_Set is_set, Eina_Bool relative)
{
	return NULL;
}

static size_t _ender_union_size_get(Ender_Descriptor *d)
{
	return 0;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Descriptor_Backend ender_union_backend = {
	/* .validate 		= */ _ender_union_validate,
	/* .creator 		= */ _ender_union_creator,
	/* .destructor 		= */ _ender_union_destructor,
	/* .property_add 	= */ _ender_union_property_add,
	/* .size_get	 	= */ _ender_union_size_get,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/


