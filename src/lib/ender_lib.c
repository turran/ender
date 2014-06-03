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
#include "ender_lib.h"

#include "ender_main_private.h"
#include "ender_lib_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Ender_Lib
{
	Eina_List *deps;
	int version;
	char *name;
	char *file;
};

static Eina_Hash *_libraries;

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_lib_init(void)
{
	_libraries = eina_hash_string_superfast_new(EINA_FREE_CB(ender_lib_free));
}

void ender_lib_shutdown(void)
{
	eina_hash_free(_libraries);
}

Ender_Lib * ender_lib_new(void)
{
	Ender_Lib *thiz;

	thiz = calloc(1, sizeof(Ender_Lib));
	thiz->version = -1;
	return thiz;
}

void ender_lib_free(Ender_Lib *thiz)
{
	eina_list_free(thiz->deps);
	free(thiz->name);
	free(thiz->file);
	free(thiz);
}

void ender_lib_version_set(Ender_Lib *thiz, int version)
{
	if (thiz->version >= 0)
	{
		ERR("Version already set");
		return;
	}
	thiz->version = version;
}

void ender_lib_name_set(Ender_Lib *thiz, const char *name)
{
	if (thiz->name)
	{
		ERR("Name already set");
		return;
	}
	if (!name)
	{
		return;
	}
	thiz->name = strdup(name);
	asprintf(&thiz->file, "lib%s.so", name);
}

void ender_lib_dependency_add(Ender_Lib *thiz, const Ender_Lib *dep)
{
	if (!thiz) return;
	if (!dep) return;
	thiz->deps = eina_list_append(thiz->deps, dep);
}

void ender_lib_register(Ender_Lib *thiz)
{
	if (!thiz) return;
	if (!thiz->name) return;

	eina_hash_add(_libraries, thiz->name, thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI int ender_lib_version_get(const Ender_Lib *thiz)
{
	if (!thiz) return -1;
	return thiz->version;
}

EAPI const char * ender_lib_name_get(const Ender_Lib *thiz)
{
	if (!thiz) return NULL;
	return thiz->name;
}

EAPI Eina_List * ender_lib_dependencies_get(const Ender_Lib *thiz)
{
	Ender_Lib *dep;
	Eina_List *ret = NULL;
	Eina_List *l;

	EINA_LIST_FOREACH(thiz->deps, l, dep)
	{
		ret = eina_list_append(ret, dep);
	}
	return ret;
}

EAPI const Ender_Lib * ender_lib_find(const char *name)
{
	return eina_hash_find(_libraries, name);
}
