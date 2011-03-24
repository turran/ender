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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct  _Ender_Library
{
	int ref;
	void *dl_handle;
	char *name;
} Ender_Library;

typedef struct _Ender_Namespace
{
	char *name;
	char *prefix;
	Ender_Library *lib;
} Ender_Namespace;

static Eina_Hash *_namespaces = NULL;
static Eina_Hash *_libraries = NULL;
static Eina_List *_files = NULL;

static void _library_unref(Ender_Library *lib)
{
	lib->ref--;
	if (!lib->ref)
	{
		dlclose(lib->dl_handle);
		free(lib);
	}
}

static void _library_ref(Ender_Library *lib)
{
	lib->ref++;
}

static void _dir_list_cb(const char *name, const char *path, void *data)
{
	char file[PATH_MAX];

	snprintf(file, PATH_MAX, "%s/%s", path, name);
	ender_parser_parse(file);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Descriptor * ender_parser_register(const char *ns, const char *name, Ender_Descriptor * parent, Ender_Type type)
{
	Ender_Descriptor *desc;
	Ender_Creator creator;
	Ender_Namespace *namespace;
	char ctor_name[PATH_MAX];

	DBG("Registering %s %s with parent %p", ns, name, parent);
	if (!ns) return NULL;

	/* check if we already have the namespace */
	namespace = eina_hash_find(_namespaces, ns);
	if (!namespace)
	{
		Ender_Library *library;
		char lib_name[PATH_MAX];
		char *tmp;

		/* Note: this wont work for multi-byte or wide chars */
		tmp = strchr(ns, '.');
		if (tmp)
		{
			strncpy(lib_name, ns, tmp - ns);
			lib_name[tmp - ns] = '\0';
		}
		else
		{
			strncpy(lib_name, ns, PATH_MAX);
			lib_name[PATH_MAX] = '\0';
		}
		/* check if we already have the library */
		library = eina_hash_find(_libraries, lib_name);
		if (!library)
		{
			char real_lib[PATH_MAX];
			void *dl_handle;

			snprintf(real_lib, PATH_MAX, "lib%s.so", lib_name);
			dl_handle = dlopen(real_lib, RTLD_LAZY);
			if (!dl_handle)
			{
				ERR("The library %s can not be found", real_lib);
				return NULL;
			}

			library = malloc(sizeof(Ender_Library));
			library->name = strdup(lib_name);
			library->dl_handle = dl_handle;
			library->ref = 0;
			/* keep a reference to the library */
			eina_hash_add(_libraries, lib_name, library);
		}
		namespace = malloc(sizeof(Ender_Namespace));
		namespace->name = strdup(ns);
		namespace->prefix = strdup(ns);
		/* replace every . with a _ */
		while ((tmp = strchr(namespace->prefix, '.')))
		{
			*tmp = '_';
		}
		namespace->lib = library;
		_library_ref(library);
		eina_hash_add(_namespaces, ns, namespace);
	}
	/* now the class itself */
	if (type == ENDER_CLASS)
	{
		snprintf(ctor_name, PATH_MAX, "%s_%s_new", namespace->prefix, name);
		creator = dlsym(namespace->lib->dl_handle, ctor_name);
		if (!creator)
		{
			DBG("No creator found?");
		}
	}
	else
	{
		creator = NULL;
	}
	desc = ender_descriptor_register(name, creator, parent, type);
	if (!desc)
	{
		free(namespace->name);
		free(namespace->prefix);
		_library_unref(namespace->lib);
		free(namespace);
		return NULL;
	}
	DBG("class %s@%s registered correctly %p", name, ns, desc);

	return desc;
}

void ender_parser_property_add(const char *ns, Ender_Descriptor *edesc,
		const char *name, Ender_Container *prop, Eina_Bool rel)
{
	Ender_Getter get;
	Ender_Setter set;
	Ender_Add add = NULL;
	Ender_Remove remove = NULL;
	Ender_Clear clear = NULL;
	Ender_Namespace *namespace;
	char prefix[PATH_MAX];
	char func_name[PATH_MAX];
	const char *edesc_name;

	if (!ns) return;

	namespace = eina_hash_find(_namespaces, ns);
	if (!namespace) return;

	edesc_name = ender_descriptor_name_get(edesc);
	DBG("Adding property %s to %s", name, edesc_name);
	snprintf(prefix, PATH_MAX, "%s_%s_%s_", namespace->prefix, edesc_name, name);

	/* the getter */
	strncpy(func_name, prefix, PATH_MAX);
	strncat(func_name, "get", PATH_MAX);
	get = dlsym(namespace->lib->dl_handle, func_name);
	if (!get)
	{
		WRN("No getter %s for type %s", func_name, edesc_name);
	}
	/* the setter */
	strncpy(func_name, prefix, PATH_MAX);
	strncat(func_name, "set", PATH_MAX);
	set = dlsym(namespace->lib->dl_handle, func_name);
	if (!set)
	{
		WRN("No setter %s for type %s", func_name, edesc_name);
	}
	/* in case of a compound property, also try to get the add/remove/clear */
	if (prop->type == ENDER_LIST)
	{
		/* the add */
		strncpy(func_name, prefix, PATH_MAX);
		strncat(func_name, "add", PATH_MAX);
		add = dlsym(namespace->lib->dl_handle, func_name);
		if (!add)
		{
			WRN("No adder %s for type %s", func_name, edesc_name);
		}
		/* the remove */
		strncpy(func_name, prefix, PATH_MAX);
		strncat(func_name, "remove", PATH_MAX);
		remove = dlsym(namespace->lib->dl_handle, func_name);
		if (!remove)
		{
			WRN("No remove %s for type %s", func_name, edesc_name);
		}
		/* the clear */
		strncpy(func_name, prefix, PATH_MAX);
		strncat(func_name, "clear", PATH_MAX);
		clear = dlsym(namespace->lib->dl_handle, func_name);
		if (!clear)
		{
			WRN("No clear %s for type %s", func_name, edesc_name);
		}
	}
	ender_descriptor_property_add(edesc, name, prop, get, set, add, remove, clear, rel);
}

void ender_parser_init(void)
{
	Eina_Iterator *it;
	Ender_Library *lib;

	_namespaces = eina_hash_string_superfast_new(NULL);
	_libraries = eina_hash_string_superfast_new(NULL);
	/* iterate over the list of .ender files and parse them */
	eina_file_dir_list(PACKAGE_DATA_DIR, EINA_FALSE, _dir_list_cb, NULL);
	/* now that every ender is regsitered we can start initializing the libraries */
	it = eina_hash_iterator_data_new(_libraries);
	while (eina_iterator_next(it, (void **)&lib))
	{
		Ender_Init init;
		char sym_name[PATH_MAX];

		/* initialize the library */
		snprintf(sym_name, PATH_MAX, "%s_init", lib->name);
		init = dlsym(lib->dl_handle, sym_name);
		if (init) init();
	}
	eina_iterator_free(it);
}

void ender_parser_shutdown(void)
{
	Eina_Iterator *it;
	Ender_Library *lib;

	/* call the shutdown on every library */
	it = eina_hash_iterator_data_new(_libraries);
	while (eina_iterator_next(it, (void **)&lib))
	{
		Ender_Init shutdown;
		char sym_name[PATH_MAX];

		/* initialize the library */
		snprintf(sym_name, PATH_MAX, "%s_shutdown", lib->name);
		shutdown = dlsym(lib->dl_handle, sym_name);
		if (shutdown) shutdown();
	}
	eina_iterator_free(it);
	/* TODO actually delete the hashes contents */
	eina_hash_free(_libraries);
	eina_hash_free(_namespaces);
	/* TODO free the list of files */
}

FILE * ender_parser_locate(const char *file)
{
	Eina_List *l;
	char *file_parsed;
	FILE *f;
	struct stat st;
	char complete[PATH_MAX];

#if 0
	/* check that we haven't parse the file already */
	EINA_LIST_FOREACH (_files, l, file_parsed)
	{
		printf("already located %s\n", file_parsed);
		if (!strcmp(file, file_parsed))
			return NULL;
	}
#endif

	strcpy(complete, file);
	if (strcmp(complete + strlen(complete) - 6, ".ender"))
	{
		strcat(complete, ".ender");
	}
	/* check for files on DATADIR */
	f = fopen(complete, "r");
	if (!f)
	{
		char real_file[PATH_MAX];

		/* TODO check if the file is relative or absolute */
		strncpy(real_file, PACKAGE_DATA_DIR, PATH_MAX);
		strncat(real_file, "/", PATH_MAX - strlen(real_file));
		strncat(real_file, complete, PATH_MAX - strlen(real_file));

		f = fopen(real_file, "r");
		if (!f)
		{
			ERR("File %s not found at . or %s", complete, real_file);
			return NULL;
		}
		DBG("Parsing file %s", real_file);
	}
	else
	{
		DBG("Parsing file %s", file);
	}
	return f;
}

void ender_parser_parse(const char *file)
{
	Ender_Parser *parser;
	FILE *f;
	int ret;
	void *scanner;

	f = ender_parser_locate(file);
	if (!f) return;

	parser = malloc(sizeof(Ender_Parser));
	ender_lex_init (&scanner);
	ender_set_in(f, scanner);
	if (!ender_parse(scanner, parser))
	{
		DBG("Parsed file %s correctly", file);
		_files = eina_list_append(_files, strdup(file));
	}

	ender_lex_destroy(scanner);
	free(parser);
	fclose(f);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

