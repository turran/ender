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
struct _Escen_Setter
{
	char *name;
	Ender_Value *value;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* TODO rename this to escen_setter_ender_apply and remove the intance parameter */
void escen_setter_instance_set(Escen_Setter *thiz, Escen_Instance *eei)
{
	Escen *e;
	Ender_Value *value;
	Ender_Container *ec;
	Ender_Property *property;
	Ender_Element *ender;
	Ender_Value_Type ptype;

	e = escen_instance_escen_get(eei);
	ender = escen_instance_ender_get(eei);
	property = ender_element_property_get(ender, thiz->name);
	if (!property)
	{
		ERR("Property \"%s\" not found", thiz->name);
		return;
	}
	ec = ender_property_container_get(property);
	ptype = ender_property_type_get(property);

	/* transform the value */
	if (!escen_copier_copy(ptype, ec, &value, thiz->value, ender_value_type_get(thiz->value)))
	{
		WRN("No way to copy the vaule");
		return;
	}

	ender_element_property_value_set_simple(ender, property, value);
	ender_value_unref(value);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
Escen_Setter * escen_setter_new(const char *name, Ender_Value *v)
{
	Escen_Setter *thiz;

	if (!name) return NULL;

	thiz = calloc(1, sizeof(Escen_Setter));
	thiz->name = strdup(name);
	thiz->value = v;

	return thiz;
}
