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
#include "ender_private.h"

#include "ender_main.h"
#include "ender_item.h"
#include "ender_lib.h"

#include "ender_main_private.h"
#include "ender_lib_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _init = 0;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int ender_log_dom = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Initialize the ender library
 */
EAPI void ender_init(void)
{
	if (!_init++)
	{
		eina_init();
		ender_log_dom = eina_log_domain_register("ender", NULL);
		ender_lib_init();
	}
}

/**
 * Shutdown the ender library
 */
EAPI void ender_shutdown(void)
{
	if (_init == 1)
	{
		ender_lib_shutdown();
		eina_log_domain_unregister(ender_log_dom);
		eina_shutdown();
	}
	_init--;
}

/**
 * Get the version
 * @param[out] major The major number of the version
 * @param[out] minor The minor number of the version
 * @param[out] micro The micro number of the version
 */
EAPI void ender_version(unsigned int *major, unsigned int *minor, unsigned int *micro)
{
	if (major) *major = VERSION_MAJOR;
	if (minor) *minor = VERSION_MINOR;
	if (micro) *micro = VERSION_MICRO;
}

