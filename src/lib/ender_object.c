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
Ender_Property * ender_object_property_add(Ender_Descriptor *edesc, const char *name,
		Ender_Container *ec, Ender_Getter get, Ender_Setter set,
		Ender_Add add, Ender_Remove remove, Ender_Clear clear,
		Ender_Is_Set is_set,
		Eina_Bool relative)
{
	Ender_Descriptor_Property *dprop;
	Ender_Property *prop;

	dprop = calloc(1, sizeof(Ender_Descriptor_Property));
	dprop->get = get;
	dprop->set = set;
	dprop->add = add;
	dprop->remove = remove;
	dprop->clear = clear;
	dprop->clear = clear;

	prop = ender_property_new(name, ec,
			get ? ender_descriptor_object_property_get : NULL,
			set ? ender_descriptor_object_property_set : NULL,
			add ? ender_descriptor_object_property_add : NULL,
			remove ? ender_descriptor_object_property_remove : NULL,
			clear ? ender_descriptor_object_property_clear : NULL,
			is_set ? ender_descriptor_object_property_is_set : NULL,
			relative,
			ender_descriptor_object_property_free, dprop);
	return prop;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
