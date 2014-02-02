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
static int _init = 0;
static Eina_Array *_modules = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int ender_log_dom = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *ENDER_ELEMENT_ENDER;
Egueb_Dom_String *ENDER_ELEMENT_INSTANCE;
Egueb_Dom_String *ENDER_ELEMENT_OBJECT;
Egueb_Dom_String *ENDER_ELEMENT_SCENE;
Egueb_Dom_String *ENDER_ELEMENT_STATES;
Egueb_Dom_String *ENDER_ELEMENT_STATE;

/**
 * Initialize the ender library
 */
EAPI void ender_init(void)
{
	if (!_init++)
	{
		egueb_dom_init();
 		ENDER_ELEMENT_ENDER = egueb_dom_string_new_with_static_string("ender");
 		ENDER_ELEMENT_INSTANCE = egueb_dom_string_new_with_static_string("instance");
 		ENDER_ELEMENT_OBJECT = egueb_dom_string_new_with_static_string("object");
 		ENDER_ELEMENT_SCENE = egueb_dom_string_new_with_static_string("scene");
 		ENDER_ELEMENT_STATES = egueb_dom_string_new_with_static_string("states");
 		ENDER_ELEMENT_STATE = egueb_dom_string_new_with_static_string("state");
		ender_log_dom = eina_log_domain_register("ender", NULL);
		ender_namespace_init();
		/* the modules */
		_modules = eina_module_list_get(_modules, PACKAGE_LIB_DIR"/ender/", 1, NULL, NULL);
		eina_module_list_load(_modules);
		/* the static modules */
#if BUILD_STATIC_MODULE_ENESIM
		ender_enesim_init();
#endif
	}
}

/**
 * Shutdown the ender library
 */
EAPI void ender_shutdown(void)
{
	if (_init == 1)
	{
		/* unload every module */
		eina_module_list_free(_modules);
		eina_array_free(_modules);
		/* unload every static module */
#if BUILD_STATIC_MODULE_ENESIM
		ender_enesim_shutdown();
#endif
		ender_namespace_shutdown();
		eina_log_domain_unregister(ender_log_dom);
		egueb_dom_string_unref(ENDER_ELEMENT_ENDER);
		egueb_dom_string_unref(ENDER_ELEMENT_INSTANCE);
		egueb_dom_string_unref(ENDER_ELEMENT_OBJECT);
		egueb_dom_string_unref(ENDER_ELEMENT_SCENE);
		egueb_dom_string_unref(ENDER_ELEMENT_STATES);
		egueb_dom_string_unref(ENDER_ELEMENT_STATE);
		egueb_dom_shutdown();
	}
	_init--;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_version(unsigned int *major, unsigned int *minor, unsigned int *micro)
{
	if (major) *major = VERSION_MAJOR;
	if (minor) *minor = VERSION_MINOR;
	if (micro) *micro = VERSION_MICRO;
}

