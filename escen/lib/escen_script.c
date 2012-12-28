/* ESCEN - Ender's based scene loader
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
#include "Escen.h"
#include "escen_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Escen_Script {
	Escen_Scriptor *scriptor;
	void *scriptor_data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void escen_script_instance_set(Escen_Script *esc, Escen_Instance *ei)
{
	Ender_Element *ender;

	ender = escen_instance_ender_get(ei);
	escen_scriptor_exec(esc->scriptor, ender, esc->scriptor_data);
}

Escen_Script * escen_script_new(Escen_Scriptor *scriptor, void *data)
{
	Escen_Script *esc;

	esc = calloc(1, sizeof(Escen_Script));
	esc->scriptor = scriptor;
	esc->scriptor_data = data;

	return esc;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_script_delete(Escen_Script *esc)
{
	free(esc);
}


