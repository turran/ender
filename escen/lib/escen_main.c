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
static int _init = 0;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int escen_log_dom = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_init(void)
{
	if (!_init++)
	{
		eina_init();
		escen_log_dom = eina_log_domain_register("escen", NULL);
		escen_copier_init();
		escen_misc_init();
		ender_init();
		etch_init();
		/* TODO get the variable from the environment */
		escen_scriptor_init();
		/* setup the copiers */
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_shutdown(void)
{
	if (!_init == 1)
	{
		escen_scriptor_shutdown();
		etch_shutdown();
		ender_shutdown();
		eina_log_domain_unregister(escen_log_dom);
		eina_shutdown();
	}
	_init--;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_version(unsigned int *major, unsigned int *minor, unsigned int *micro)
{
	if (major) *major = VERSION_MAJOR;
	if (minor) *minor = VERSION_MINOR;
	if (micro) *micro = VERSION_MICRO;
}
