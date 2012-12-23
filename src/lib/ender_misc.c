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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * ender_descriptor_type_string_to(Ender_Descriptor_Type type)
{
	switch (type)
	{
		case ENDER_TYPE_ABSTRACT:
		return "ABSTRACT";

		case ENDER_TYPE_CLASS:
		return "CLASS";

		case ENDER_TYPE_STRUCT:
		return "STRUCT";

		case ENDER_TYPE_UNION:
		return "UNION";

		default:
		return NULL;
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_descriptor_type_value_type_to(Ender_Descriptor_Type d, Ender_Value_Type *v)
{
	Eina_Bool ret = EINA_TRUE;

	switch (d)
	{
		case ENDER_TYPE_ABSTRACT:
		*v = ENDER_OBJECT;
		break;

		case ENDER_TYPE_CLASS:
		*v = ENDER_OBJECT;
		break;

		case ENDER_TYPE_STRUCT:
		*v = ENDER_STRUCT;
		break;

		case ENDER_TYPE_UNION:
		*v = ENDER_UNION;
		break;

		default:
		ret = EINA_FALSE;
		break;
	}
	return ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * ender_value_type_string_to(Ender_Value_Type type)
{
	switch (type)
	{
		case ENDER_UINT32:
		return "UINT32";

		case ENDER_INT32:
		return "INT32";

		case ENDER_UINT64:
		return "UINT64";

		case ENDER_INT64:
		return "INT64";

		case ENDER_DOUBLE:
		return "DOUBLE";

		case ENDER_COLOR:
		return "COLOR";

		case ENDER_ARGB:
		return "ARGB";

		case ENDER_STRING:
		return "STRING";

		case ENDER_MATRIX:
		return "MATRIX";

		case ENDER_OBJECT:
		return "OBJECT";

		case ENDER_SURFACE:
		return "SURFACE";

		case ENDER_ENDER:
		return "ENDER";

		case ENDER_LIST:
		return "LIST";

		case ENDER_STRUCT:
		return "STRUCT";

		case ENDER_UNION:
		return "UNION";

		case ENDER_BOOL:
		return "BOOL";

		case ENDER_POINTER:
		return "POINTER";

		default:
		return "UNKNOWN";
	}
}
