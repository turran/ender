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
#include "ender_item.h"
#include "ender_lib.h"
#include "ender_value.h"
#include "ender_parser.h"

#include "ender_main_private.h"
#include "ender_lib_private.h"
#include "ender_item_private.h"
#include "ender_item_basic_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/** @cond internal */
struct _Ender_Lib
{
	Eina_List *deps;
	Eina_Hash *items;
	int version;
	char *name;
	char *file;
	void *dl;
};

static Eina_Hash *_libraries = NULL;
static Ender_Lib *_c_lib = NULL;
static int _init = 0;

static void _ender_lib_dir_list_cb(const char *name, const char *path, void *data)
{
	FILE *f;
	const Ender_Lib *lib;
	char *file = NULL;
	char *token;
	char *fname;

	token = strchr(name, '.');
	if (!token) return;

	fname = strndup(name, token - name);
	lib = ender_lib_find(fname);
	if (lib)
	{
		INF("Library '%s' already parsed", fname);
		free(fname);
		return;
	}
	free(fname);

	if (asprintf(&file, "%s/%s", path, name) < 0)
		return;

	DBG("Parsing file '%s'", file);
	f = fopen(file, "r");
	ender_parser_parse(f);
	fclose(f);
	free(file);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_lib_init(void)
{
	if (!_init++)
	{
		Ender_Item *i;

		_libraries = eina_hash_string_superfast_new(
				EINA_FREE_CB(ender_lib_free));
		/* add the main c lib */
		_c_lib = ender_lib_new();
		ender_lib_name_set(_c_lib, "c");
		/* bool */
		i = ender_item_basic_new();
		ender_item_name_set(i, "bool");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_BOOL);
		ender_lib_item_add(_c_lib, i);
		/* uint8 */
		i = ender_item_basic_new();
		ender_item_name_set(i, "uint8");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_UINT8);
		ender_lib_item_add(_c_lib, i);
		/* int8 */
		i = ender_item_basic_new();
		ender_item_name_set(i, "int8");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_INT8);
		ender_lib_item_add(_c_lib, i);
		/* uint32 */
		i = ender_item_basic_new();
		ender_item_name_set(i, "uint32");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_UINT32);
		ender_lib_item_add(_c_lib, i);
		/* int32 */
		i = ender_item_basic_new();
		ender_item_name_set(i, "int32");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_INT32);
		ender_lib_item_add(_c_lib, i);
		/* uint64 */
		i = ender_item_basic_new();
		ender_item_name_set(i, "uint64");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_UINT64);
		ender_lib_item_add(_c_lib, i);
		/* int64 */
		i = ender_item_basic_new();
		ender_item_name_set(i, "int64");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_INT64);
		ender_lib_item_add(_c_lib, i);
		/* double */
		i = ender_item_basic_new();
		ender_item_name_set(i, "double");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_DOUBLE);
		ender_lib_item_add(_c_lib, i);
		/* string */
		i = ender_item_basic_new();
		ender_item_name_set(i, "string");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_STRING);
		ender_lib_item_add(_c_lib, i);
		/* pointer */
		i = ender_item_basic_new();
		ender_item_name_set(i, "pointer");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_POINTER);
		ender_lib_item_add(_c_lib, i);
		/* pointer */
		i = ender_item_basic_new();
		ender_item_name_set(i, "size");
		ender_item_basic_value_type_set(i, ENDER_VALUE_TYPE_SIZE);
		ender_lib_item_add(_c_lib, i);
		/* load all the libs on the data dir */
		eina_file_dir_list(DESCRIPTIONS_DIR, EINA_FALSE, _ender_lib_dir_list_cb, NULL);
	}
}

void ender_lib_shutdown(void)
{
	if (_init == 1)
	{
		eina_hash_free(_libraries);
		ender_lib_free(_c_lib);
	}
	_init--;
}

Ender_Lib * ender_lib_new(void)
{
	Ender_Lib *thiz;

	thiz = calloc(1, sizeof(Ender_Lib));
	thiz->items = eina_hash_string_superfast_new(EINA_FREE_CB(ender_item_unref));
	thiz->version = -1;
	return thiz;
}

void ender_lib_free(Ender_Lib *thiz)
{
	eina_hash_free(thiz->items);
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
	if (asprintf(&thiz->file, "lib%s.so", name) < 0)
		return;
}

void ender_lib_dependency_add(Ender_Lib *thiz, const Ender_Lib *dep)
{
	if (!thiz) return;
	if (!dep) return;
	thiz->deps = eina_list_append(thiz->deps, dep);
}

void ender_lib_register(Ender_Lib *thiz)
{
	Ender_Lib *other;

	if (!thiz) return;
	if (!thiz->name) return;

	other = eina_hash_find(_libraries, thiz->name);
	if (other)
	{
		WRN("Library '%s' already registered", thiz->name);
	}

	DBG("Registering lib '%s'", thiz->name);
	eina_hash_add(_libraries, thiz->name, thiz);
}

void ender_lib_item_add(Ender_Lib *thiz, Ender_Item *i)
{
	const char *name;

	if (!i) return;
	if (!thiz)
	{
		ender_item_unref(i);
		return;
	}

	name = ender_item_name_get(i);
	if (!name)
	{
		ender_item_unref(i);
		return;
	}

	DBG("Adding item %p '%s' %d on '%s' lib", i, name, ender_item_type_get(i), thiz->name);
	i->lib = thiz;
	eina_hash_add(thiz->items, name, i);
}

void ender_lib_load(Ender_Lib *thiz)
{
	if (thiz->dl)
		return;

	DBG("Loading lib '%s'", thiz->file);
	thiz->dl = dlopen(thiz->file, RTLD_LAZY);
	if (!thiz->dl)
	{
		CRI("Impossible to load the library '%s'", thiz->file);
	}
}

void * ender_lib_sym_get(Ender_Lib *thiz, const char *name)
{
	ender_lib_load(thiz);
	DBG("Loading sym '%s' from lib '%s'", name, thiz->name);
	return dlsym(thiz->dl, name);
}
/** @endcond */
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Find a registered library by name
 * @param name The name of the library to find
 * @return The library found, NULL otherwise
 */
EAPI const Ender_Lib * ender_lib_find(const char *name)
{
	return eina_hash_find(_libraries, name);
}

/**
 * Get the version of the library
 * @param thiz The library to get version from
 * @return The version of the library
 */
EAPI int ender_lib_version_get(const Ender_Lib *thiz)
{
	if (!thiz) return -1;
	return thiz->version;
}

/**
 * Get the name of the library
 * @param thiz The library to get name from
 * @return The name of the library
 */
EAPI const char * ender_lib_name_get(const Ender_Lib *thiz)
{
	if (!thiz) return NULL;
	return thiz->name;
}

/**
 * Get the dependencies of the library
 * @param thiz The library to get the dependencies from
 * @return The list of dependencies or NULL if it does not have any.
 * Make sure to free the returning list
 */
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

EAPI Ender_Item * ender_lib_item_find(const Ender_Lib *thiz, const char *name)
{
	Ender_Item *i;
	Ender_Lib *dep;
	Eina_List *l;

	if (!thiz) return NULL;

	i = eina_hash_find(thiz->items, name);
	if (i) return ender_item_ref(i);

	/* check on the dependencies */
	EINA_LIST_FOREACH(thiz->deps, l, dep)
	{
		/* do not make it too recursive */
		i = eina_hash_find(dep->items, name);
		if (i) return i;
	}

	/* check on the c lib */
	if (thiz != _c_lib)
	{
		return ender_lib_item_find(_c_lib, name);
	}

	return NULL;
}

EAPI Eina_List * ender_lib_item_list(const Ender_Lib *thiz, Ender_Item_Type type)
{
	Ender_Item *i;
	Eina_List *ret = NULL;
	Eina_Iterator *it;

	if (!thiz) return ret;

	it = eina_hash_iterator_data_new(thiz->items);
	EINA_ITERATOR_FOREACH(it, i)
	{
		if (ender_item_type_get(i) == type)
			ret = eina_list_append(ret, ender_item_ref(i));
	}

	eina_iterator_free(it);
	return ret;
}
