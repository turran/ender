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
#include "ender_main_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef char * (*Ender_Name_Convert)(const char *s);

static char * _ender_utils_name_convert_case_underscore_latin_camel_english(
		const char *s)
{
	return NULL;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI char * ender_utils_name_convert(const char *s, Ender_Case src_case,
		Ender_Notation src_not, Ender_Case dst_case, Ender_Notation dst_not)
{
	static Ender_Name_Convert _conv[ENDER_CASE_TYPES][ENDER_NOTATION_TYPES]
			[ENDER_CASE_TYPES][ENDER_NOTATION_TYPES];
	static Eina_Bool _init = EINA_FALSE;
	Ender_Name_Convert conv;

	if (!_init)
	{
		_conv[ENDER_CASE_UNDERSCORE][ENDER_NOTATION_LATIN]
				[ENDER_CASE_CAMEL][ENDER_NOTATION_ENGLISH] =
				_ender_utils_name_convert_case_underscore_latin_camel_english;
		_init = EINA_TRUE;
	}

	conv = _conv[src_case][src_not][dst_case][dst_not];
	if (!conv) return NULL;

	return conv(s);
}
