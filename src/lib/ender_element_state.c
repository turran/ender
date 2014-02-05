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
#include "Ender.h"
#include "ender_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                      The exernal element interface                         *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _ender_element_state_tag_name_get(
		Egueb_Dom_Node *node, void *data)
{
	return egueb_dom_string_ref(ENDER_ELEMENT_STATE);
}

static Egueb_Dom_Element_External_Descriptor _descriptor = {
	/* init 		= */ NULL,
	/* deinit 		= */ NULL,
	/* tag_name_get		= */ _ender_element_state_tag_name_get,
	/* process 		= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * ender_element_state_new(void)
{
	return egueb_dom_element_external_new(&_descriptor, NULL);
}

