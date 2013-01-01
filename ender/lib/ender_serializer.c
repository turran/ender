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
#include "Ender.h"
#include "ender_private.h"

/* In order to serialize values we need:
 * 1. Simple container types (int, double, etc) just require the
 * main eet data descriptor to be the Ender_Value_Data union
 * so we just "decode" into it directly
 * 2. Lists, require that the children are allocated and thus
 * we need to create a data descriptor for each basic type. In case the
 * list has a compound type like a struct or union we need to
 * be able to create such malloced data by eet itself
 * 3. Unions/Structs requires eet to define its descriptor dynamically
 * based on the children container type not the container descriptor
 * 4. Pointers (enders, objects, surfaces, etc) require the user to
 * provide an "id" number for them. This "id" must be provided
 * and calculated by the user. Given that it is the most
 * difficult part, it will be done at the end.
 *
 * Problems with eet tha ive seen so far:
 * 1. You can only serialize full structs, not single values
 * 2. Given the nature of the compound containers (structs and unions)
 * where you can add elements incrementally, you need to know beforehand
 * the full siz eof your struct which is false of course. An option
 * is to destroy the previous descriptor and create a new one, but then
 * every container referencing this (struct inside struct) must be aware of
 * it
 * Other alternatives:
 * http://tpl.sourceforge.net/userguide.html
 * http://gwlib.com/
 * http://s11n.net/c11n/
 * http://avro.apache.org/docs/current/api/c/index.html
 *
 * Ok, now that we have committed tpl here, we need to start doing the
 * encoding/decoding of the values.
 * 1. Every container must have a string referring to its mapping
 * 2. Looks like we can unmap single struct fields, if that's the case
 * when unmapping structs, instead of unmapping the whole struct just iterate
 * over it an decode single values, when a an object is decoded, trigger the
 * callback. We can mark a struct as if it has or not a foreign reference
 *
 * We need to avoid the user to modify a container after it is assigned
 * into a property. So we can fixate a container when it is assigned.
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* FIXME for now we just use one possibility, later we can have a way to define
 * what serializer to use?
 */
Ender_Serializer * _ender_serializer_selected_get(void)
{
	Ender_Serializer *thiz = NULL;

	thiz = ender_serializer_eet_get();
	if (thiz) return thiz;

#if 0
	thiz = ender_serializer_tpl_get();
	if (thiz) return thiz;
#endif
	return thiz;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * ender_serializer_container_new(Ender_Container *ec)
{
	Ender_Serializer *thiz;

	thiz = _ender_serializer_selected_get();
	if (!thiz) return NULL;
	if (!thiz->container_new)
		return NULL;
	return thiz->container_new(ec);
}

void * ender_serializer_element_marshal(Ender_Element *e, unsigned int *len)
{
	Ender_Serializer *thiz;
	Ender_Descriptor *d;

	thiz = _ender_serializer_selected_get();
	if (!thiz) return NULL;
	if (!thiz->element_marshal)
		return NULL;
	d = ender_element_descriptor_get(e);
	return thiz->element_marshal(d, e, len);
}

Ender_Element * ender_serializer_element_unmarshal(Ender_Descriptor *d,
		void *data, unsigned int len)
{
	Ender_Serializer *thiz;

	thiz = _ender_serializer_selected_get();
	if (!thiz) return NULL;
	if (!thiz->element_unmarshal)
		return NULL;
	return thiz->element_unmarshal(d, data, len);
}

Ender_Value * ender_serializer_value_unmarshal(Ender_Container *ec,
		void *data, unsigned int len)
{
	Ender_Serializer *thiz;

	thiz = _ender_serializer_selected_get();
	if (!thiz) return NULL;
	if (!thiz->value_unmarshal)
		return NULL;
	return thiz->value_unmarshal(ec->serialize_data, data, len);
}

void * ender_serializer_value_marshal(const Ender_Value *v, unsigned int *len)
{
	Ender_Serializer *thiz;

	thiz = _ender_serializer_selected_get();
	if (!thiz) return NULL;
	if (!thiz->value_marshal)
		return NULL;
	return thiz->value_marshal(v->container->serialize_data, v, len);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
