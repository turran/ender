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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Property_Container * ender_property_container_new(Ender_Property_Type t)
{
	Ender_Property_Container *prop;

	prop = malloc(sizeof(Ender_Property));
	prop->type = t;
	prop->sub = NULL;
	switch (t)
	{
		case ENDER_LIST:
		prop->sub = eina_array_new(1);
		break;

		default:
		break;
	}
	return prop;
}

void ender_property_container_delete(Ender_Property_Container *d)
{
	if (d->sub)
	{
		/* call the delete for each descriptor */
	}
	free(d);
}

void ender_property_container_add(Ender_Property_Container *d, Ender_Property_Container *sub)
{
	if (d->type != ENDER_LIST) return;
	if (!d->sub) return;
	eina_array_push(d->sub, sub);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 *
 */
EAPI Ender_Property_Type ender_property_type(Ender_Property *p)
{
	return p->prop->type;
}

/**
 *
 */
EAPI Eina_Bool ender_property_is_relative(Ender_Property *p)
{
	return p->relative;
}

/**
 *
 */
EAPI const Eina_Array * ender_property_sub(Ender_Property *p)
{
	return p->prop->sub;
}

/**
 *
 */
EAPI const char * ender_property_name_get(Ender_Property *p)
{
	return p->name;
}
