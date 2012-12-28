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
static Eina_Hash *_scriptors = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void escen_scriptor_register(Escen_Scriptor *es, const char *type)
{
	eina_hash_add(_scriptors, type, es);
}

void escen_scriptor_init(void)
{
	_scriptors = eina_hash_string_superfast_new(NULL);
#if BUILD_SCRIPT_NEKO
#if 0
	if (escen_script_neko_init())
	{
		escen_scriptor_register(&escen_scriptor_neko, "neko");
	}
#endif
#endif
}

void escen_scriptor_shutdown(void)
{
	if (_scriptors)
		eina_hash_free(_scriptors);
}

void * escen_scriptor_load(const char *script_text, const char *type, Escen_Scriptor **scriptor)
{
	Escen_Scriptor *current;

	if (!scriptor) return NULL;

	current = eina_hash_find(_scriptors, type);
	if (!current) return NULL;
	*scriptor = current;

	return current->load(script_text);
}

void escen_scriptor_exec(Escen_Scriptor *es, Ender_Element *e, void *data)
{
	es->exec(e, data);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

