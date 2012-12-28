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
/* TODO
 * - whenever a state is set/unset send an event on the ender instance
 * - We need to support an action section on each state to be able to trigger
 * some actions like setting a new state on another escen_ender, mainly
 * a escen_ender that is a property of another
 * - We need to add some flags/properties to the escen itself, like
 * setting the fps or maybe the engine used for creating surfaces (for this
 * case the best solution might be to add an external callback like the
 * surface loader)
 * - Add a way for states to inherit from another state, this way we can ease
 * the animations where the state of a propery might have an invalid value
 * - Add a scripting language, like we did for old eon, something like neko, js
 * lua or whatever, what we need is to just provide ender and escen API, that's
 * all
 * - Whenever a surface is loaded we should have a surface cache
 * - we need a way to know whenever a escen ender
 * is part of another escen ender
 * - Add functions to add callbacks whenever an instance is:
 *   + created (DONE)
 *   + deleted
 *   + modified (like the fps, or whatever)
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Escen
{
	char *name;
	char *version;
	unsigned int fps;
	Eina_Ordered_Hash *enders;
	Eina_Array *paths;
};

static Eina_Hash *_image_cache = NULL;

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen * escen_new(void)
{
	Escen *e;

	e = calloc(1, sizeof(Escen));
	e->fps = 30;
	e->enders = eina_ordered_hash_new(NULL);
	e->paths = eina_array_new(1);

	return e;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_delete(Escen *e)
{
	/* TODO check if we have a renderer property, if so
	 * delete the ender associated to it too
	 * we'll need to add some kind of data set on the
	 * enesim_renderer by name
	 */
	eina_array_free(e->paths);
	eina_ordered_hash_free(e->enders);
	free(e);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_fps_set(Escen *e, unsigned int fps)
{
	e->fps = fps;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI unsigned int escen_fps_get(Escen *e)
{
	return e->fps;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_surface_path_append(Escen *e, const char *path)
{
	eina_array_push(e->paths, strdup(path));
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Ender * escen_ender_add(Escen *e, Ender_Descriptor *descriptor, const char *name)
{
	Escen_Ender *ee;

	if (!e || !name) return NULL;

	ee = escen_ender_new(e, descriptor, name);
	if (!ee) return NULL;

	eina_ordered_hash_add(e->enders, name, ee);

	return ee;
}

/**
 * Gets the number of Escen_Enders an Escen has
 * @param[in] e The Escen
 * @return The number of Enders
 */
EAPI int escen_ender_count(Escen *e)
{
	if (!e) return 0;
	return eina_ordered_hash_count(e->enders);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Ender * escen_ender_get(Escen *e, const char *name)
{
	Escen_Ender *ee;

	if (!name) return NULL;
	if (!e) return NULL;
	ee = eina_ordered_hash_find(e->enders, name);

	return ee;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Ender * escen_ender_nth_get(Escen *e, int nth)
{
	if (!e) return NULL;
	return eina_ordered_hash_nth_get(e->enders, nth);
}
