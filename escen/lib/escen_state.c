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
struct _Escen_State
{
	char *name;
	Escen_Ender *ee;
	Eina_List *sets;
	Eina_List *animations;
	Eina_List *scripts;
	Eina_List *inherits;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Escen_State * escen_state_new(Escen_Ender *ee, const char *name)
{
	Escen_State *es;

	es = calloc(1, sizeof(Escen_State));
	es->name = strdup(name);
	es->ee = ee;

	return es;
}

void escen_state_instance_setters_set(Escen_State *es, Escen_Instance *eei)
{
	Escen_Setter *set;
	Eina_List *l;

	EINA_LIST_FOREACH(es->sets, l, set)
	{
		escen_setter_instance_set(set, eei);
	}
}

void escen_state_instance_animations_set(Escen_State *es, Escen_Instance *eei,
		Escen_Time time)
{
	Escen_Animation *animation;
	Eina_List *l;

	EINA_LIST_FOREACH(es->animations, l, animation)
	{
		escen_animation_instance_set(animation, eei, time);
	}
}

void escen_state_instance_scripts_set(Escen_State *es, Escen_Instance *eei)
{
	Escen_Script *script;
	Eina_List *l;

	EINA_LIST_FOREACH(es->scripts, l, script)
	{
		escen_script_instance_set(script, eei);
	}

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Ender * escen_state_ender_get(Escen_State *es)
{
	if (!es) return NULL;

	return es->ee;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * escen_state_name_get(Escen_State *es)
{
	if (!es) return NULL;

	return es->name;
}

/**
 * TODO we should create an Escen_Value to reflect the case where adding an escen_ender
 * as a value
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_state_setter_add(Escen_State *es, Escen_Setter *et)
{
	if (!es) return;
	if (!et) return;

	es->sets = eina_list_append(es->sets, et);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Script * escen_state_script_add(Escen_State *es, const char *type, const char * script)
{
	Escen_Script *esc;
	Escen_Scriptor *scriptor;
	void *scriptor_data;

	if (!es) return NULL;

	scriptor_data = escen_scriptor_load(script, type, &scriptor);
	if (!scriptor) return NULL;

	esc = escen_script_new(scriptor, scriptor_data);

	es->scripts = eina_list_append(es->scripts, esc);
	return esc;
}

/* TODO add a setter value set, that should update the current value, in case theres an instance
 * of this escen_ender and is on this state, update the property too */

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_state_animation_add(Escen_State *es, Escen_Animation *ea)
{
	if (!es) return;
	if (!ea) return;

	es->animations = eina_list_append(es->animations, ea);
}
