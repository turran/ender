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

static Eina_Bool _is_verb(const char *s, int len)
{
	Eina_Bool is_verb = EINA_FALSE;

	if (!strncmp(s, "get", len))
	{
		is_verb = EINA_TRUE;
	}
	else if (!strncmp(s, "set", len))
	{
		is_verb = EINA_TRUE;
	}
	else if (!strncmp(s, "create", len))
	{
		is_verb = EINA_TRUE;
	}
	else if (!strncmp(s, "append", len))
	{
		is_verb = EINA_TRUE;
	}

	return is_verb;
}

/* name_get -> get_name
 * name_get_id_by -> get_name_by_id
 * new_data_from -> new_from_data
 * foo_new -> new_foo
 * foo_new_data_from -> new_foo_from_data
 */
static char * _ender_utils_name_convert_latin_english(const char *s)
{
	Eina_Bool swap = EINA_FALSE;
	const char *tmp;
	char *ret;
	size_t len;

	/* iterate until we find a verb */
	tmp = strchr(s, '_');

	while (tmp)
	{
		const char *stmp;

		tmp = tmp + 1;
		/* get the length of the verb */
		stmp = strchr(tmp, '_');
		if (!stmp)
			len = strlen(tmp);
		else
			len = stmp - tmp;

		swap = _is_verb(tmp, len);
		if (swap)
			break;
		tmp = stmp;
	}
	/* if none found, just exit */
	if (!swap)
		return strdup(s);

	ret = calloc(strlen(s) + 1, sizeof(char));
	/* copy the verb */
	strncpy(ret, tmp, len);
	/* add the unerscore (so far verb_) */
	ret[len] = '_';
	/* finally add everything before the verb without the trailing '_'*/
	strncpy(ret + len + 1, s, tmp - s - 1);
	ret[len + 1 + (tmp - s - 1)] = '\0';

	/* iterate until we find a modifier */
	return ret;
}

static char * _ender_utils_name_convert_english_latin(const char *s)
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

	/* check if first word is a verb */
	swap = _is_verb(s, len);

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

static char * _ender_utils_case_convert_underscore_camel_pascal(
		const char *s, Eina_Bool pascal)
{
	Eina_Bool swap = pascal;
	char *dtmp;
	char *d;
	const char *stmp;
	const char *send;
	size_t len;

	len = strlen(s);
	send = s + len;

	d = calloc(len + 1, sizeof(char));
	dtmp = d;
	for (stmp = s; stmp < send; stmp++)
	{
		if (*stmp == '_')
		{
			swap = EINA_TRUE;
			continue;
		}

		if (swap)
		{
			*dtmp = toupper(*stmp);
			swap = EINA_FALSE;
		}
		else
		{
			*dtmp = *stmp;
		}
		dtmp++;
	}

	return d;
}

/* replace every char following '_' by the upper case char */
static char * _ender_utils_case_convert_underscore_camel(
		const char *s)
{
	return _ender_utils_case_convert_underscore_camel_pascal(s, EINA_FALSE);
}

static char * _ender_utils_case_convert_underscore_pascal(
		const char *s)
{
	return _ender_utils_case_convert_underscore_camel_pascal(s, EINA_TRUE);
}

static char * _ender_utils_case_convert_camel_underscore(
		const char *s)
{
	Eina_Bool is_upper = EINA_FALSE;
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
			if (!is_upper)
			{
				*dtmp = '_';
				dtmp++;
			}

			*dtmp = tolower(*stmp);
			dtmp++;

			is_upper = EINA_TRUE;
		}
		else
		{
			/* for SVGPoint -> svg_point */
			if (is_upper)
			{
				if ((dtmp - 2 > d) && (*(dtmp - 2) != '_'))
				{
					*dtmp = *(dtmp - 1);
					*(dtmp - 1) = '_';
					dtmp++;
				}
			}

			*dtmp = *stmp;
			dtmp++;

			is_upper = EINA_FALSE;
		}
	}

	return d;
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
	static Ender_Name_Convert _cconv[ENDER_CASE_TYPES][ENDER_CASE_TYPES];
	static Ender_Name_Convert _nconv[ENDER_NOTATION_TYPES][ENDER_NOTATION_TYPES];
	static Eina_Bool _init = EINA_FALSE;
	Ender_Name_Convert cconv;
	Ender_Name_Convert nconv;
	char *tmp;
	char *ret;

	if (!_init)
	{
		_cconv[ENDER_CASE_UNDERSCORE][ENDER_CASE_CAMEL] =
				_ender_utils_case_convert_underscore_camel;
		_cconv[ENDER_CASE_UNDERSCORE][ENDER_CASE_PASCAL] =
				_ender_utils_case_convert_underscore_pascal;
		_cconv[ENDER_CASE_CAMEL][ENDER_CASE_UNDERSCORE] = 
			_ender_utils_case_convert_camel_underscore;

		_nconv[ENDER_NOTATION_LATIN][ENDER_NOTATION_ENGLISH] =
				_ender_utils_name_convert_latin_english;
		_nconv[ENDER_NOTATION_ENGLISH][ENDER_NOTATION_LATIN] =
				_ender_utils_name_convert_english_latin;
		_init = EINA_TRUE;
	}

	nconv = _nconv[src_not][dst_not];
	cconv = _cconv[src_case][dst_case];

	if (!nconv || !cconv)
		return NULL;

	/* first the case, we only work with underscore */
	if (src_case != ENDER_CASE_UNDERSCORE)
	{
		tmp = cconv(s);
		if (!tmp)
			return NULL;

		ret = nconv(tmp);
		free(tmp);
	}
	else
	{
		tmp = nconv(s);
		if (!tmp)
			return NULL;

		ret = cconv(tmp);
		free(tmp);
	}

	return ret;
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
