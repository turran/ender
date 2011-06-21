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
 *
 */
EAPI const char * ender_type_name_get(Ender_Type type)
{
	switch (type)
	{
		case ENDER_ABSTRACT:
		return "ABSTRACT";

		case ENDER_CLASS:
		return "CLASS";

		default:
		return NULL;
	}
}

/**
 *
 */
EAPI const char * ender_value_type_name_get(Ender_Value_Type type)
{
	switch (type)
	{
		case ENDER_UINT32:
		return "UINT32";

		case ENDER_INT32:
		return "INT32";

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

		case ENDER_RENDERER:
		return "RENDERER";

		case ENDER_SURFACE:
		return "SURFACE";

		case ENDER_ENDER:
		return "ENDER";

		case ENDER_LIST:
		return "LIST";

		case ENDER_STRUCT:
		return "STRUCT";

		case ENDER_BOOL:
		return "BOOL";

		default:
		return NULL;
	}
}
