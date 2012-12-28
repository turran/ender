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
struct _Escen_Instance
{
	Ender_Element *ender;
	Etch *etch;
	Escen_Ender *ee;
	Escen_State *current;
	int current_wait;
	Eina_List *animation_instances;
};

typedef struct _Escen_Instance_New_Listener
{
	Escen_Instance_New_Callback cb;
	void *data;
} Escen_Instance_New_Listener;

static Eina_List *_instance_new_listeners = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Etch * escen_instance_etch_get(Escen_Instance *ei)
{
	return ei->etch;
}

void escen_instance_wait_increment(Escen_Instance *ei)
{
	ei->current_wait++;
}

void escen_instance_wait_decrement(Escen_Instance *ei)
{
	ei->current_wait--;
}

void escen_instance_animation_instance_add(Escen_Instance *ei, Escen_Animation_Instance *eai)
{
	ei->animation_instances = eina_list_append(ei->animation_instances, eai);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Instance * escen_instance_new(Escen_Ender *e, Eina_Bool set_default)
{
	Escen_Instance *eei;
	Escen *escen;
	Escen_Instance_New_Listener *listener;
	Ender_Element *ender;
	Ender_Descriptor *descriptor;
	Eina_List *l;

	if (!e) return NULL;

	descriptor = escen_ender_descriptor_get(e);
	if (!descriptor) return NULL;

	ender = ender_descriptor_element_new(descriptor);
	if (!ender) return NULL;

	eei = calloc(1, sizeof(Escen_Instance));
	eei->ee = e;
	eei->ender = ender;
	eei->etch = etch_new();
	escen = escen_ender_escen_get(e);
	etch_timer_fps_set(eei->etch, escen_fps_get(escen));
	/* setup the ender */
	escen_ender_instance_setup(e, eei);
	if (set_default)
	{
		Escen_State *default_state;

		/* load the default state */
		default_state = escen_ender_state_get(e, "default");
		if (default_state)
			escen_instance_state_set(eei, default_state);
	}

	/* call the callbacks */
	EINA_LIST_FOREACH(_instance_new_listeners, l, listener)
	{
		listener->cb(eei, listener->data);
	}

	return eei;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_instance_new_callback_add(Escen_Instance_New_Callback cb, void *data)
{
	Escen_Instance_New_Listener *listener;

	if (!cb) return;
	listener = malloc(sizeof(Escen_Instance_New_Listener));
	listener->cb = cb;
	listener->data = data;

	_instance_new_listeners = eina_list_append(_instance_new_listeners, listener);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * escen_instance_ender_get(Escen_Instance *eei)
{
	return eei->ender;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Ender * escen_instance_escen_ender_get(Escen_Instance *eei)
{
	return eei->ee;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen * escen_instance_escen_get(Escen_Instance *eei)
{
	return escen_ender_escen_get(eei->ee);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_instance_state_set(Escen_Instance *eei, Escen_State *es)
{
	Escen_Animation_Instance *eai;
	Escen_Time curr;
	Escen_Script *script;
	Ender_Element *ender;
	Eina_List *l;

	if (!es) return;
	if (escen_state_ender_get(es) != eei->ee) return;

	/* TODO what to do with the previous state? */
	EINA_LIST_FOREACH(eei->animation_instances, l, eai)
	{
		/* FIXME really delete the animation instance */
		etch_animation_disable(eai->eta);
	}

	/* first set the setters */
	escen_state_instance_setters_set(es, eei);
	/* now set the animations */
	etch_timer_get(eei->etch, &curr);
	escen_state_instance_animations_set(es, eei, curr);
	/* now call the scripts */
	escen_state_instance_scripts_set(es, eei);
	eei->current = es;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_State * escen_instance_current_state_get(Escen_Instance *ei)
{
	return ei->current;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool escen_instance_current_state_finalized(Escen_Instance * ei)
{
	if (!ei) return EINA_TRUE;
	if (!ei->current) return EINA_TRUE;

	if (!ei->current_wait) return EINA_TRUE;
	return EINA_FALSE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_instance_process(Escen_Instance *eei)
{
	etch_timer_tick(eei->etch);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_instance_delete(Escen_Instance *eei)
{

}


