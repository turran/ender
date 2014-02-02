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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Ender.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Namespace * _ender_enesim_namespace = NULL;
static int _ender_enesim_log = -1;
/*----------------------------------------------------------------------------*
 *                                 Renderer                                   *
 *----------------------------------------------------------------------------*/
static void _ender_enesim_renderer_init(Egueb_Dom_Node *n)
{

}

static void _ender_enesim_renderer_deinit(Egueb_Dom_Node *n, void *data)
{

}

/*----------------------------------------------------------------------------*
 *                                  Shape                                     *
 *----------------------------------------------------------------------------*/
static void _ender_enesim_renderer_shape_init(Egueb_Dom_Node *n)
{

}
/*----------------------------------------------------------------------------*
 *                                 Circle                                     *
 *----------------------------------------------------------------------------*/
static void * _ender_enesim_renderer_circle_init(Egueb_Dom_Node *n)
{
	Enesim_Renderer *r;
	Egueb_Dom_Node *a;

	/* the properties */
	a = ender_attr_double_new("x",
			ENDER_ATTR_DOUBLE_GET(enesim_renderer_circle_x_get),
			ENDER_ATTR_DOUBLE_SET(enesim_renderer_circle_x_set));
	egueb_dom_element_attribute_add(n, a, NULL);

	a = ender_attr_double_new("y",
			ENDER_ATTR_DOUBLE_GET(enesim_renderer_circle_y_get),
			ENDER_ATTR_DOUBLE_SET(enesim_renderer_circle_y_set));
	egueb_dom_element_attribute_add(n, a, NULL);

	a = ender_attr_double_new("radius",
			ENDER_ATTR_DOUBLE_GET(enesim_renderer_circle_radius_get),
			ENDER_ATTR_DOUBLE_SET(enesim_renderer_circle_radius_set));
	egueb_dom_element_attribute_add(n, a, NULL);

	/* the object */
	r = enesim_renderer_circle_new();
	return r;
}

static void _ender_enesim_renderer_circle_deinit(Egueb_Dom_Node *n, void *data)
{
	enesim_renderer_unref(data);
}

static Ender_Instance_Descriptor _ender_enesim_renderer_circle_descriptor = {
	/* init 	 	= */ _ender_enesim_renderer_circle_init,
	/* deinit 	 	= */ _ender_enesim_renderer_circle_deinit,
	/* process	 	= */ NULL,
	/* child_appendable	= */ NULL,
	/* child_added		= */ NULL,
	/* child_removed	= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                               Module API                                   *
 *----------------------------------------------------------------------------*/
#if BUILD_STATIC_MODULE_ENESIM
Eina_Bool ender_enesim_init(void)
#else
static Eina_Bool ender_enesim_init(void)
#endif
{
	_ender_enesim_log = eina_log_domain_register("ender_enesim", NULL);
	if (_ender_enesim_log < 0)
	{
		EINA_LOG_ERR("Ender: Can not create enesim log domain.");
		return EINA_FALSE;
	}
	_ender_enesim_namespace = ender_namespace_register("enesim");
	ender_namespace_instance_register(_ender_enesim_namespace, &_ender_enesim_renderer_circle_descriptor, "circle");
	if (!_ender_enesim_namespace)
		return EINA_FALSE;
	return EINA_TRUE;
}

#if BUILD_STATIC_MODULE_ENESIM
void ender_enesim_shutdown(void)
#else
static void ender_enesim_shutdown(void)
#endif
{
	ender_namespace_unregister(_ender_enesim_namespace);
	_ender_enesim_namespace = NULL;

	eina_log_domain_unregister(_ender_enesim_log);
	_ender_enesim_log = -1;
}

#if !BUILD_STATIC_MODULE_ENESIM
EINA_MODULE_INIT(ender_enesim_init);
EINA_MODULE_SHUTDOWN(ender_enesim_shutdown);
#endif

