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

static Eina_Hash *_namespaces = NULL;
static Eina_Hash *_libraries = NULL;
extern FILE *ender_in;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Array *properties = NULL;

Ender_Descriptor * ender_parser_register(const char *ns, const char *name, Ender_Descriptor * parent)
{
	Ender_Descriptor *desc;
	Ender_Creator creator;
	Ender_Namespace *namespace;
	char ctor_name[PATH_MAX];

	DBG("Registering %s %s %p", ns, name, parent);
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
		}
		else
		{
			strncpy(lib_name, ns, PATH_MAX); 
		}
		/* check if we already have the library */
		library = eina_hash_find(_libraries, lib_name);
		if (!library)
		{
			char real_lib[PATH_MAX];
			void *dl_handle;

			snprintf(real_lib, PATH_MAX, "lib%s.so", lib_name);
			dl_handle = dlopen(real_lib, RTLD_LAZY);
			if (!dl_handle) return NULL;

			library = malloc(sizeof(Ender_Library));
			library->name = strdup(lib_name);
			library->dl_handle = dl_handle;
			library->ref = 0;
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
	snprintf(ctor_name, PATH_MAX, "%s_%s_new", namespace->prefix, name);
	creator = dlsym(namespace->lib->dl_handle, ctor_name);
	/* an interface? */
	if (!creator)
	{
		printf("no creator found?\n");
	}
	desc = ender_descriptor_register(name, creator, parent);
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
		const char *name, Ender_Property *prop)
{
	Ender_Getter get;
	Ender_Setter set;
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
	ender_descriptor_property_add(edesc, name, prop, get, set);
}

void ender_parser_init(void)
{
	_namespaces = eina_hash_string_superfast_new(NULL);
	_libraries = eina_hash_string_superfast_new(NULL);
	/* FIXME create a proper context */
	properties = eina_array_new(1);
	/* iterate over the list of .ender files and parse them */
	eina_file_dir_list(PACKAGE_DATA_DIR, EINA_FALSE, _dir_list_cb, NULL);
}

void ender_parser_shutdown(void)
{
	/* dlclose every namespace */
	eina_hash_free(_namespaces);
}

void ender_parser_parse(const char *file)
{
	FILE *f;
	int ret;
	struct stat st;

	/* check for files on DATADIR */
	f = fopen(file, "r");
	if (!f)
	{
		char real_file[PATH_MAX];

		/* TODO check if the file is relative or absolute */
		strncpy(real_file, PACKAGE_DATA_DIR, PATH_MAX);
		strncat(real_file, "/", PATH_MAX - strlen(real_file));
		strncat(real_file, file, PATH_MAX - strlen(real_file));

		f = fopen(file, "r");
		ERR("file %s not found at . or %s", file, real_file);
		if (!f) return;
	}

	ender_in = f;
	ret = ender_parse();
	fclose(f);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

