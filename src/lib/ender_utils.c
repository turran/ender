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
	tmp = strchr(s, '_');

	/* no separator */
	if (!tmp) return strdup(s);

	len = tmp - s;
	if (!strncmp(s, "get", len))
	{
		swap = EINA_TRUE;
	}
	else if (!strncmp(s, "set", len))
	{
		swap = EINA_TRUE;
	}

	/* ok we got a verb */
	if (swap)
	{
		Eina_Bool extra = EINA_FALSE;
		char *stmp;
		size_t llen;

		/* check if we have a modifier, like from, by, etc */
		stmp = strchr(tmp, '_');
		while (stmp)
		{
			char *sstmp;

			stmp++;
			sstmp = strchr(stmp, '_');
			if (sstmp)
			{
				size_t sslen;

				sslen = sstmp - stmp;
				if (!strncmp(stmp, "by", sslen))
				{
					extra = EINA_TRUE;
					break;
				}
				else if (!strncmp(stmp, "from", sslen))
				{
					extra = EINA_TRUE;
					break;
				}
			}
			stmp = sstmp;
		}

		ret = calloc(strlen(s) + 1, sizeof(char));
		/* subject_verb_extra */
		if (extra)
		{
			llen = stmp - tmp - 1;
			strncpy(ret, tmp + 1, llen);
			strncpy(ret + llen, s, len + 1);
			strcpy(ret + llen + len + 1, stmp);

		}
		/* subject_verb */
		else
		{
			/* copy the verb */
			strcpy(ret, tmp + 1);
			llen = strlen(ret);
			ret[llen] = '_';
			strncpy(ret + llen + 1, s, len);
		}
	}
	else
	{
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

EAPI char * ender_utils_to_lower(const char *str)
{
	char *ret;
	char *tmp;
	size_t len;

	len = strlen(str);
	ret = tmp = malloc(len + 1);
	while (*str)
	{
		*tmp = tolower(*str);
		tmp++;
		str++;
	}
	ret[len] = '\0';
	return ret;
}

EAPI char * ender_utils_to_upper(const char *str)
{
	char *ret;
	char *tmp;
	size_t len;

	len = strlen(str);
	ret = tmp = malloc(len + 1);
	while (*str)
	{
		*tmp = toupper(*str);
		tmp++;
		str++;
	}
	ret[len] = '\0';
	return ret;
}
