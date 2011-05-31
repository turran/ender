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

/**
 * @todo
 *  - We need a way to register enders without the *need* of parsing,
 *  that is, do it programmatically
 *  - Add a color property, rename the current argb property to color
 *  and be able to parse both Enesim_Color and Enesim_Argb this is useful
 *  for the gradient case where the input MUST be on argb unpremultiplied
 */
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
		enesim_init();
		ender_descriptor_init();
		ender_container_init();
		ender_parser_init();
	}
}

/**
 * Shutdown the ender library
 */
EAPI void ender_shutdown(void)
{
	if (_init == 1)
	{
		ender_parser_shutdown();
		ender_container_shutdown();
		ender_descriptor_shutdown();
		enesim_shutdown();
		eina_log_domain_unregister(ender_log_dom);
		eina_shutdown();
	}
	_init--;
}
