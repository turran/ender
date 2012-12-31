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

#if BUILD_SERIALIZER_TPL
#include "tpl.h"
#endif

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#if BUILD_SERIALIZER_TPL
typedef struct _Ender_Container_Serialize
{
	char *signature;
	Eina_Bool external;
} Ender_Container_Serialize;

static void _ender_container_serialize_add(Ender_Value_Type t)
{
	switch (t)
	{
		case ENDER_BOOL:
		break;

		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		break;

		case ENDER_INT32:
		break;

		case ENDER_UINT64:
		case ENDER_OBJECT:
		case ENDER_ENDER:
		case ENDER_POINTER:
		break;

		case ENDER_INT64:
		break;

		case ENDER_DOUBLE:
		break;

		case ENDER_STRING:
		break;

		/* the compound sub types will be added on its own function */
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		break;

		case ENDER_VALUE:
		ERR("value not supported yet");
		break;
	}
}

static void _ender_container_serialize_new(Ender_Container_Serialize *s, Ender_Value_Type t)
{
	switch (t)
	{
		case ENDER_BOOL:
		s->signature = "c";
		break;

		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		s->signature = "u";
		break;

		case ENDER_INT32:
		s->signature = "i";
		break;

		case ENDER_UINT64:
		s->signature = "U";
		break;

		/* for this we need an id */
		case ENDER_OBJECT:
		case ENDER_ENDER:
		case ENDER_POINTER:
		s->signature = "U";
		s->external = EINA_TRUE;
		break;

		case ENDER_INT64:
		s->signature = "I";
		break;

		case ENDER_DOUBLE:
		s->signature = "f";
		break;

		case ENDER_STRING:
		s->signature = "s";
		break;

		/* the compound sub types will be added on its own function */
		case ENDER_LIST:
		s->signature = "A()";
		break;

		case ENDER_STRUCT:
		case ENDER_UNION:
		s->signature = "S()";
		break;

		case ENDER_VALUE:
		WRN("value not supported yet");
		break;
	}
}
/*----------------------------------------------------------------------------*
 *                       The serializer interface                             *
 *----------------------------------------------------------------------------*/
static void * _tpl_value_marshal()
{
	tpl_node *n;
	void *data = NULL;

	switch (thiz->type)
	{
		case ENDER_INT32:
		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_BOOL:
		case ENDER_INT64:
		case ENDER_UINT64:
		case ENDER_DOUBLE:
		case ENDER_STRING:
		n = tpl_map(thiz->serialize.signature, &v->data);
		tpl_pack(n, 0);
		tpl_dump(n, TPL_MEM, &data, len);
		tpl_free(n);
		break;

		/* for external objects, fetch it into a local var
		 * then call the callback and set it on the value
		 */
		case ENDER_ENDER:
		case ENDER_POINTER:
		case ENDER_LIST:
		ERR("Marshaling of type %s is not supported yet",
				ender_value_type_string_to(thiz->type));
		break;

		/* for structs/unions allocate the needed size and assign it to the void * */
		/* for lists, iterate over each element on the array and add it to the list, then assign the
		 * the list to the void * */
		case ENDER_OBJECT:
		case ENDER_STRUCT:
		case ENDER_UNION:
		{
			const Ender_Constraint *c;

			c = ender_container_constraint_get(thiz);
			if ((!c) || (ender_constraint_type_get(c) !=
					ENDER_CONSTRAINT_DESCRIPTOR))
			{
				ERR("For type %s we need a descriptor "
						"constraint or we can not "
						"identify the members",
						ender_value_type_string_to(
						thiz->type));
				break;
			}
			/* now marshal the native object */
			
		}
		break;
	}
	return data;
}

static void * _tpl_value_unmarshal()
{
	Ender_Value *v;
	tpl_node *n;

	v = ender_value_new_container_from(thiz);
	switch (thiz->type)
	{
		case ENDER_INT32:
		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_BOOL:
		case ENDER_INT64:
		case ENDER_UINT64:
		case ENDER_DOUBLE:
		case ENDER_STRING:
		n = tpl_map(thiz->serialize.signature, &v->data);
		tpl_load(n, TPL_MEM, data, len);
		tpl_unpack(n, 0);
		tpl_free(n);
		break;

		/* for external objects, fetch it into a local var
		 * then call the callback and set it on the value
		 */
		case ENDER_OBJECT:
		case ENDER_ENDER:
		case ENDER_POINTER:

		/* for structs/unions allocate the needed size and assign it to the void * */
		/* for lists, iterate over each element on the array and add it to the list, then assign the
		 * the list to the void * */
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		break;
	}
	return v;
}

static Ender_Serializer _ender_serializer_tpl = {

};
#endif
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Serializer * ender_serializer_tpl_get(void)
{
#if BUILD_SERIALIZER_TPL
	return &_ender_serializer_tpl;
#else
	return NULL;
#endif
}
