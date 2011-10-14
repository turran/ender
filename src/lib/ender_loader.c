/* ENDER - Enesim's descriptor library
 * Copyright (C) 2010 - 2011 Jorge Luis Zapata
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
#ifndef _WIN32
#include <unistd.h>
#endif
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_List *_pre_registry = NULL;
static int _init = 0;

typedef struct _Ender_Loader_Registry_Data
{
	Ender_Loader_Registry_Callback cb;
	void *data;
} Ender_Loader_Registry_Data;

typedef struct _Ender_Library
{
	int ref;
	void *dl_handle;
	char *name;
} Ender_Library;

typedef struct _Ender_Library_Namespace
{
	Ender_Library *lib;
	Ender_Namespace *ns;
} Ender_Library_Namespace;

typedef struct _Ender_Loader
{
	char *file;
	Ender_Library_Namespace *namespace;
	Ender_Descriptor *descriptor;
} Ender_Loader;

static Eina_Hash *_libraries = NULL;
static Eina_Hash *_library_namespaces = NULL;
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
	ender_loader_load(file);
}

static Ender_Library * _library_new(const char *name, void *dl_handle)
{
	Ender_Library *library;

	library = malloc(sizeof(Ender_Library));
	library->name = strdup(name);
	library->dl_handle = dl_handle;
	library->ref = 0;

	return library;
}

static Eina_Bool _file_locate(const char *file, char *real_file)
{
	struct stat st;

	strcpy(real_file, file);
	if (strcmp(real_file + strlen(real_file) - 6, ".ender"))
	{
		strcat(real_file, ".ender");
	}
	/* check for files on DATADIR */
	if (stat(real_file, &st) < 0)
	{
		char *tmp;

		tmp = strdup(real_file);
		/* TODO check if the file is relative or absolute */
		strncpy(real_file, PACKAGE_DATA_DIR, PATH_MAX);
		strncat(real_file, "/", PATH_MAX - strlen(real_file));
		strncat(real_file, tmp, PATH_MAX - strlen(real_file));
		free(tmp);

		if (stat(real_file, &st) < 0)
		{
			ERR("File %s.ender not found at . or %s", file, real_file);
			return EINA_FALSE;
		}
	}

	DBG("Parsing file %s", real_file);
	return EINA_TRUE;
}

/* a name is the name used on "using" and "namespace", that is, somthing like
 * enesim.renderer which should be translated to
 * lib = enesim
 * ns = renderer
 */
static void _name_split(char *name, char **lib, char **ns)
{
	char *tmp;

	tmp = strchr(name, '.');
	if (tmp)
	{
		printf("dot found %s\n", tmp);
	}
	/* no dot found, everythgin is the same */
	else
	{
		*lib = tmp;
		*ns = tmp;
	}
}

/* replace every dot with underscores */
static void _name_to_c(char *name)
{
	char *tmp;

	while ((tmp = strchr(name, '.')))
	{
		*tmp = '_';
	}
}

static Ender_Library_Namespace * _loader_namespace_new(const char *name)
{
	Ender_Library_Namespace *namespace;
	Ender_Namespace *ns;
	Ender_Library *library;
	char tmp1[PATH_MAX];
	char *tmp2;

	if (!name) return NULL;
	DBG("Registering new namespace %s", name);
	/* check if we already have the namespace */
	namespace = eina_hash_find(_library_namespaces, name);
	if (namespace) return namespace;

	/* first we need to split the namespace by the dot 
	 * the string before the first dot is the library name 
	 * and the rest is the namespace 
	 * Note: this wont work for multi-byte or wide chars
	 */
	tmp2 = strchr(name, '.');
	if (tmp2)
	{
		strncpy(tmp1, name, tmp2 - name);
		tmp1[tmp2 - name] = '\0';
	}
	else
	{
		strncpy(tmp1, name, PATH_MAX);
		tmp1[PATH_MAX] = '\0';
	}
	/* check if we already have the library */
	library = eina_hash_find(_libraries, tmp1);
	if (!library)
	{
		char real_lib[PATH_MAX];
		void *dl_handle;

		snprintf(real_lib, PATH_MAX, "lib%s.so", tmp1);
		dl_handle = dlopen(real_lib, RTLD_NOW);
		if (!dl_handle)
		{
			ERR("The library %s can not be found", real_lib);
			return NULL;
		}
		library = _library_new(tmp1, dl_handle);
		eina_hash_add(_libraries, tmp1, library);
	}
	/* replace every . with a _ */
	strncpy(tmp1, name, PATH_MAX);
	while ((tmp2 = strchr(tmp1, '.')))
	{
		*tmp2 = '_';
	}
	_library_ref(library);
	ns = ender_namespace_find(tmp1);
	if (!ns)
	{
		ns = ender_namespace_new(tmp1);
	}

	namespace = malloc(sizeof(Ender_Library_Namespace));
	namespace->ns = ns;
	namespace->lib = library;

	return namespace;
}

Ender_Descriptor * _loader_descriptor_new(Ender_Library_Namespace *namespace, const char *name, Ender_Descriptor *parent, Ender_Type type)
{
	Ender_Descriptor *desc;
	Ender_Creator creator;
	const char *ns_name;

	if (!namespace) return NULL;

	ns_name = ender_namespace_name_get(namespace->ns);
	/* now the class itself */
	if (type == ENDER_CLASS)
	{
		char ctor_name[PATH_MAX];
		
		snprintf(ctor_name, PATH_MAX, "%s_%s_new", ns_name, name);
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
	desc = ender_namespace_descriptor_add(namespace->ns, name, creator, parent, type);
	if (!desc) return NULL;
	DBG("class %s@%s registered correctly %p", name, ns_name, desc);

	return desc;
}

void _loader_descriptor_property_add(Ender_Library_Namespace *namespace, Ender_Descriptor *edesc,
		const char *name, Ender_Container *prop, Eina_Bool rel)
{
	Ender_Getter get;
	Ender_Setter set;
	Ender_Add add = NULL;
	Ender_Remove remove = NULL;
	Ender_Clear clear = NULL;
	char prefix[PATH_MAX];
	char func_name[PATH_MAX];
	const char *edesc_name;
	const char *ns_name;

	if (!namespace || !namespace->ns || !namespace->lib) return;

	ns_name = ender_namespace_name_get(namespace->ns);
	edesc_name = ender_descriptor_name_get(edesc);
	DBG("Adding property %s to %s", name, edesc_name);
	snprintf(prefix, PATH_MAX, "%s_%s_%s_", ns_name, edesc_name, name);

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

/*----------------------------------------------------------------------------*
 *                           The parser insterface                            *
 *----------------------------------------------------------------------------*/
static void _loader_on_using(void *data, const char *file)
{
	char *c_name;

	char *lib;
	char *ns;

	c_name = strdup(file);
	_name_to_c(c_name);
	ender_loader_load(c_name);
	free(c_name);
}

static void _loader_on_namespace(void *data, const char *name)
{
	Ender_Loader *thiz;

	thiz = data;
	thiz->namespace = _loader_namespace_new(name);
}

static void _loader_on_renderer(void *data, const char *name, Ender_Type type, const char *parent)
{
	Ender_Loader *thiz;
	Ender_Descriptor *parent_descriptor = NULL;

	thiz = data;
	if (parent)
	{
		parent_descriptor = ender_descriptor_find(parent);
		if (!parent_descriptor)
		{
			ERR("No parent %s found for desriptor %s", parent, name);
			return;
		}
	}
	thiz->descriptor = _loader_descriptor_new(thiz->namespace, name, parent_descriptor, type);
}

static void _loader_on_property(void *data, const char *name, Eina_Bool relative, Ender_Container *container)
{
	Ender_Loader *thiz;

	thiz = data;
	_loader_descriptor_property_add(thiz->namespace, thiz->descriptor, name, container, relative);

}

static Ender_Parser_Descriptor _loader_parser = {
	/* on_using = */ _loader_on_using,
	/* on_namespace = */ _loader_on_namespace,
	/* on_renderer = */ _loader_on_renderer,
	/* on_property = */ _loader_on_property,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_loader_init(void)
{
	if (!_init++)
	{
		_library_namespaces = eina_hash_string_superfast_new(NULL);
		_libraries = eina_hash_string_superfast_new(NULL);
	}
}

void ender_loader_shutdown(void)
{
	if (_init-- == 1)
	{
		Eina_Iterator *it;
		Ender_Library *lib;
		/* call the shutdown on every library */
		it = eina_hash_iterator_data_new(_libraries);
		while (eina_iterator_next(it, (void **)&lib))
		{
			Ender_Init shutdown;
			char sym_name[PATH_MAX];

			/* shutdown the library */
			snprintf(sym_name, PATH_MAX, "%s_shutdown", lib->name);
			shutdown = dlsym(lib->dl_handle, sym_name);
			if (shutdown) shutdown();
		}
		eina_iterator_free(it);
		/* TODO actually delete the hashes contents */
		eina_hash_free(_libraries);
		eina_hash_free(_library_namespaces);
		/* TODO free the list of files */
	}
}

void ender_loader_load_all(void)
{
	Ender_Library *lib;
	Ender_Loader_Registry_Data *data;
	Eina_Iterator *it;
	Eina_List *l;


	/* first call the pre callbacks */
	EINA_LIST_FOREACH(_pre_registry, l, data)
	{
		data->cb(data->data);
	}

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
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_loader_load(const char *in)
{
	Ender_Loader loader;
	Eina_List *l;
	char *file_parsed;
	char real_file[PATH_MAX];

	loader.namespace = NULL;
	loader.descriptor = NULL;
	if (!_file_locate(in, real_file))
		return;

	/* check that we haven't parse the file already */
	EINA_LIST_FOREACH (_files, l, file_parsed)
	{
		if (!strcmp(real_file, file_parsed))
		{
			DBG("File already parsed %s", real_file);
			return;
		}
	}
	if (ender_parser_parse(real_file, &_loader_parser, &loader))
	{
		DBG("Parsed file %s correctly", real_file);
		_files = eina_list_append(_files, strdup(real_file));
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_loader_registry_callback_add(Ender_Loader_Registry_Callback cb, void *data)
{
	Ender_Loader_Registry_Data *pre;

	ender_loader_init();

	pre = calloc(1, sizeof(Ender_Loader_Registry_Data));
	pre->cb = cb;
	pre->data = data;

	_pre_registry = eina_list_append(_pre_registry, pre);
}
