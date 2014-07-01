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

#include <ctype.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef char * (*Ender_Name_Convert)(const char *s);

static char * _ender_utils_name_convert_english_latin(char *s)
{
	Eina_Bool swap = EINA_FALSE;
	const char *tmp;
	char *ret;
	size_t len;

	/* check for the verb */
	tmp = strtok(s, "_");

	/* no separator */
	if (!tmp) return strdup(s);

	if (!strcmp(s, "get"))
	{
		swap = EINA_TRUE;
	}
	else if (!strcmp(s, "set"))
	{
		swap = EINA_TRUE;
	}

	len = strlen(tmp);
	if (swap)
	{

		ret = calloc(strlen(s) + 1, sizeof(char));

		strcpy(ret, tmp + len + 1);
		len = strlen(ret);
		ret[len] = '_';
		strcpy(ret + len + 1, s);
	}
	else
	{
		s[len] = '_';
		ret = strdup(s);
	}

	return ret;
}

static char * _ender_utils_name_convert_underscore_latin_camel_english(
		const char *s)
{
	return NULL;
}

static char * _ender_utils_name_convert_camel_english_underscore_latin(
		const char *s)
{
	char *ret;
	char *dtmp;
	char *d;
	const char *stmp;
	const char *send;
	size_t len;

	/* the worst case is one a_b_c for aBC, i.e 2*len - 1 + NULL */
	len = strlen(s);
	send = s + len;

	d = calloc((len * 2), sizeof(char));
	dtmp = d;
	
	for (stmp = s; stmp < send; stmp++)
	{
		if (isupper(*stmp))
		{
			*dtmp = '_';
			dtmp++;

			*dtmp = tolower(*stmp);
			dtmp++;
		}
		else
		{
			*dtmp = *stmp;
			dtmp++;
		}
	}

	ret = _ender_utils_name_convert_english_latin(d);
	free(d);

	return ret;
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
				_ender_utils_name_convert_underscore_latin_camel_english;
		_conv[ENDER_CASE_CAMEL][ENDER_NOTATION_ENGLISH]
				[ENDER_CASE_UNDERSCORE][ENDER_NOTATION_LATIN] =
				_ender_utils_name_convert_camel_english_underscore_latin;
		_init = EINA_TRUE;
	}

	conv = _conv[src_case][src_not][dst_case][dst_not];
	if (!conv) return NULL;

	return conv(s);
}
