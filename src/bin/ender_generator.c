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

#include <ctype.h>

#include "Ender.h"
#include "ender_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Ender_Generator
{
	char *ns_name;
	int ns_version;
	const char *name;
	FILE *out;
	Eina_Bool found;
	Eina_Bool name_matched;
	Eina_Bool extended;
	Eina_Bool container_initialized;
} Ender_Generator;

static void _tabs(Ender_Generator *thiz, int count)
{
	int i;

	for (i = 0; i < count; i++)
		fprintf(thiz->out, "\t");
}

static void _upper(const char *src, size_t len, char *dst)
{
	int i;

	for (i = 0; i < len; i++)
	{
		char d;
		char s = *src++;

		if (isalpha(s))
			d = toupper(s);
		else
			d = s;
		*dst++ = d;
	}
}

static void _property_variable_name(char *dst, const char *ns, const char *oname, const char *pname)
{
	size_t len;

	len = strlen(ns);
	_upper(ns, len, dst);
	dst += len;
	*dst++ = '_';

	len = strlen(oname);
	_upper(oname, len, dst);
	dst += len;
	*dst++ = '_';
	
	len = strlen(pname);
	_upper(pname, len, dst);
	dst += len;
	*dst++ = '\0';
}

static void _property_normalized(char *dst, const char *name)
{
	const char *tmp;

	tmp = name;
	while (tmp && *tmp)
	{
		/* TODO replace this with the isalpha, isnumber, etc variants */
		if (*tmp == ' ' || *tmp == '-' || *tmp == ':')
		{
			*dst = '_';
		}
		else
			*dst = *tmp;
		dst++;
		tmp++;
	}
	*dst = '\0';
}

static void _dump_container_recursive(Ender_Generator *thiz, Ender_Container *container, int level)
{
	Ender_Value_Type type;

	type = ender_container_type_get(container);
	_tabs(thiz, level);
	fprintf(thiz->out, "Ender_Container *tmp%d;\n", level);
	_tabs(thiz, level);
	if (type == ENDER_STRUCT || type == ENDER_UNION)
	{
		const char *registered_name;

		registered_name = ender_container_registered_name_get(container);
		fprintf(thiz->out, "tmp%d = ender_container_find(\"%s\");\n", level, registered_name);
	}
	else
	{
		fprintf(thiz->out, "tmp%d = ender_container_new(ENDER_%s);\n", level, ender_value_type_string_to(type));
		if (type == ENDER_LIST)
		{
			Ender_Container *sub;
			const char *name;

			ender_container_compound_get_extended(container, 0, &sub, &name);
			_tabs(thiz, level + 1);
			fprintf(thiz->out, "{\n");
			_dump_container_recursive(thiz, sub, level + 2);
			_tabs(thiz, level + 2);
			if (name && *name)
				fprintf(thiz->out, "ender_container_add(tmp%d, \"%s\", tmp%d);\n", level, name, level + 2);
			else
				fprintf(thiz->out, "ender_container_add(tmp%d, NULL, tmp%d);\n", level, level + 2);
				
			_tabs(thiz, level + 1);
			fprintf(thiz->out, "}\n");
		}
	}
}

static void _dump_container_struct_union_recursive(Ender_Generator *thiz, Ender_Container *container, int level)
{
	Ender_Value_Type type;
	int i;

	type = ender_container_type_get(container);
	_tabs(thiz, level);
	fprintf(thiz->out, "Ender_Container *tmp%d;\n", level);
	_tabs(thiz, level);
	fprintf(thiz->out, "tmp%d = ender_container_new(ENDER_%s);\n", level, ender_value_type_string_to(type));

	_tabs(thiz, level);
	fprintf(thiz->out, "{\n");
	for (i = 0; i < ender_container_compound_count(container); i++)
	{
		Ender_Container *sub;
		const char *name;

		ender_container_compound_get_extended(container, i, &sub, &name);
		_tabs(thiz, level + 1);
		fprintf(thiz->out, "{\n");
		_dump_container_recursive(thiz, sub, level + 2);
		_tabs(thiz, level + 2);
		if (name && *name)
			fprintf(thiz->out, "ender_container_add(tmp%d, \"%s\", tmp%d);\n", level, name, level + 2);
		else
			fprintf(thiz->out, "ender_container_add(tmp%d, NULL, tmp%d);\n", level, level + 2);
			
		_tabs(thiz, level + 1);
		fprintf(thiz->out, "}\n");
	}
	_tabs(thiz, level);
	fprintf(thiz->out, "}\n");
}

/*----------------------------------------------------------------------------*
 *                            The parser interface                            *
 *----------------------------------------------------------------------------*/
static void _generator_on_namespace(void *data, const char *name, int version)
{
	Ender_Generator *thiz;

	thiz = data;
	thiz->ns_name = strdup(name);
	thiz->ns_version = version;
}

static void _generator_on_container(void *data, const char *name, Ender_Container *c)
{
	Ender_Generator *thiz;

	thiz = data;
	if (!thiz->container_initialized)
	{
		fprintf(thiz->out, "/* this file is automatically generated */\n");
		fprintf(thiz->out, "void %s_types_init(void)\n", thiz->ns_name);
		fprintf(thiz->out, "{\n");
		thiz->container_initialized = EINA_TRUE;
		thiz->found = EINA_TRUE;
	}
	/* register the container found */
	fprintf(thiz->out, "\t{\n");
	_dump_container_struct_union_recursive(thiz, c, 2);
	fprintf(thiz->out, "\t\tender_container_register(tmp2, \"%s\");\n", name);
	fprintf(thiz->out, "\t}\n");
}

static void _generator_on_object(void *data, const char *name, Ender_Descriptor_Type type, const char *parent)
{
	Ender_Generator *thiz;

	thiz = data;
	if (strcmp(name, thiz->name))
	{
		thiz->name_matched = EINA_FALSE;
		return;
	}
	thiz->name_matched = EINA_TRUE;
	thiz->found = EINA_TRUE;
	fprintf(thiz->out, "/* this file is automatically generated */\n");
	fprintf(thiz->out, "void %s_%s_init(void)\n", thiz->ns_name, thiz->name);
	fprintf(thiz->out, "{\n");
	fprintf(thiz->out, "\tEnder_Namespace *ns;\n");
	fprintf(thiz->out, "\tEnder_Descriptor *parent = NULL;\n");
	fprintf(thiz->out, "\tstatic Ender_Descriptor *d = NULL;\n");
	fprintf(thiz->out, "\tif (d) return;\n");
	fprintf(thiz->out, "\tns = %s_namespace_get();\n", thiz->ns_name);
	/* TODO instead of getting the descriptor this way, get it from every "using"
	 * also using the version
	 */
	/* the descriptor itself */
	if (parent)
	{
		fprintf(thiz->out, "\tparent = ender_namespace_descriptor_find(ns, \"%s\");\n", parent);
		fprintf(thiz->out, "\tif (!parent) parent = ender_descriptor_find(\"%s\");;\n", parent);
		fprintf(thiz->out, "\tif (!parent) return;\n");
	}
	if (type == ENDER_ABSTRACT)
		fprintf(thiz->out, "\td = ender_namespace_descriptor_add(ns, \"%s\", NULL, ENDER_DESTRUCTOR(_%s_%s_delete), parent, ENDER_%s);\n",
				thiz->name, thiz->ns_name, thiz->name,
				ender_descriptor_type_string_to(type));
	else
		fprintf(thiz->out, "\td = ender_namespace_descriptor_add(ns, \"%s\", ENDER_CREATOR(_%s_%s_new), ENDER_DESTRUCTOR(_%s_%s_delete), parent, ENDER_%s);\n",
				thiz->name, thiz->ns_name, name, thiz->ns_name, name,
				ender_descriptor_type_string_to(type));
	fprintf(thiz->out, "\tif (!d) return;\n");
}

static void _generator_on_property(void *data, const char *name, Eina_Bool relative, Ender_Container *container)
{
	Ender_Generator *thiz;
	Ender_Value_Type type;
	char pname[PATH_MAX];
	char pnormalized[PATH_MAX];

	thiz = data;
	if (!thiz->name_matched) return;

	/* generate the normalized name */
	_property_normalized(pnormalized, name);
	/* first the container */
	fprintf(thiz->out, "\t{\n");
	fprintf(thiz->out, "\t\tEnder_Container *ec;\n");
	fprintf(thiz->out, "\t\tEnder_Property *ep;\n");
	_dump_container_recursive(thiz, container, 2);
	fprintf(thiz->out, "\t\tec = tmp2;\n");
	fprintf(thiz->out, "\t\tep = ender_descriptor_property_add(d, \"%s\",\n", name);
	fprintf(thiz->out, "\t\t\t\tec,\n");
	fprintf(thiz->out, "\t\t\t\tENDER_GETTER(_%s_%s_%s_get),\n", thiz->ns_name, thiz->name, pnormalized);
	fprintf(thiz->out, "\t\t\t\tENDER_SETTER(_%s_%s_%s_set),\n", thiz->ns_name, thiz->name, pnormalized);
	type = ender_container_type_get(container);
	if (type == ENDER_LIST)
	{
		fprintf(thiz->out, "\t\t\t\tENDER_ADD(_%s_%s_%s_add),\n", thiz->ns_name, thiz->name, pnormalized);
		fprintf(thiz->out, "\t\t\t\tENDER_REMOVE(_%s_%s_%s_remove),\n", thiz->ns_name, thiz->name, pnormalized);
		fprintf(thiz->out, "\t\t\t\tENDER_CLEAR(_%s_%s_%s_clear),\n", thiz->ns_name, thiz->name, pnormalized);
	}
	else
	{
		fprintf(thiz->out, "\t\t\t\tNULL,\n");
		fprintf(thiz->out, "\t\t\t\tNULL,\n");
		fprintf(thiz->out, "\t\t\t\tNULL,\n");
	}
	/* the is_set is part of the extended functions */
	if (thiz->extended)
		fprintf(thiz->out, "\t\t\t\tENDER_IS_SET(_%s_%s_%s_is_set),\n", thiz->ns_name, thiz->name, pnormalized);
	else
		fprintf(thiz->out, "\t\t\t\tNULL,\n");
	fprintf(thiz->out, "\t\t\t\t%s\n", relative ? "EINA_TRUE" : "EINA_FALSE");
	fprintf(thiz->out, "\t\t\t\t);\n");
	/* now assing the exported property variable to the recently created property */
	_property_variable_name(pname, thiz->ns_name, thiz->name, pnormalized);
	fprintf(thiz->out, "\t\t%s = ep;\n", pname, pname);
	/* close everything */
	fprintf(thiz->out, "\t}\n");
}

static Ender_Parser_Descriptor _generator_parser = {
	/* .on_using 		= */ NULL,
	/* .on_namespace 	= */ _generator_on_namespace,
	/* .on_object 		= */ _generator_on_object,
	/* .on_property 	= */ _generator_on_property,
	/* .on_container 	= */ NULL,
};

static Ender_Parser_Descriptor _generator_type_parser = {
	/* .on_using 		= */ NULL,
	/* .on_namespace 	= */ _generator_on_namespace,
	/* .on_object 		= */ NULL,
	/* .on_property 	= */ NULL,
	/* .on_container 	= */ _generator_on_container,
};

static void _generator_run(Eina_Bool extended, const char *i, const char *element, const char *o)
{
	Ender_Generator thiz;
	FILE *out;

	out = fopen(o, "w");

	thiz.name = element;
	thiz.out = out;
	thiz.found = EINA_FALSE;
	thiz.name_matched = EINA_FALSE;
	thiz.extended = extended;
	thiz.container_initialized = EINA_FALSE;

	if (!strcmp(element, "types"))
		ender_parser_parse(i, &_generator_type_parser, &thiz);
	else
		ender_parser_parse(i, &_generator_parser, &thiz);

	if (thiz.found)
	{
		fprintf(thiz.out, "}\n");
	}

	if (thiz.ns_name)
		free(thiz.ns_name);
}

/*
 * The idea here is to generate the .c code from an ender descriptor
 * file  (.ender). This way we can avoid the need to parse the file
 * everytime when ender initialize and register the different
 * namespaces statically
 */

static void help(void)
{
	printf("Run: ender_generator OPTION INPUT NAME OUTPUT\n");
	printf("Where:\n");
	printf("OPTION can be one of the following:\n");
	printf("  x Use the extended properties (is_set and unset)\n");
	printf("INPUT is the ender file to load\n");
	printf("NAME is the element name to dump. If you use the special\n");
	printf("  keyword 'type' it will only dump the new container types\n");
	printf("and OUTPUT is the c file to save\n");
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int main(int argc, char **argv)
{
	ender_init(&argc, &argv);
	/* now parse the file input output */
	if (argc < 4 || argc > 5)
	{
		help();
		return -1;
	}

	if (argc == 5)
	{
		if (*argv[1] != 'x')
		{
			help();
			return -1;
		}
		_generator_run(EINA_TRUE, argv[2], argv[3], argv[4]);
	}
	else
	{
		_generator_run(EINA_FALSE, argv[1], argv[2], argv[3]);
	}

	ender_shutdown();

	return 0;
}

