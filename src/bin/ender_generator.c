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

/* some function to filter the names of things
 * TODO add a way to camelize/uncamelize
 */
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

	printf("%s: %s %s %s\n", __FUNCTION__, ns, oname, pname);

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

static void _find_container(Ender_Generator *thiz,
		Ender_Parser_Container *c, int level)
{
	_tabs(thiz, level);
	fprintf(thiz->out, "Ender_Descriptor *tmp%d = ender_namespace_descriptor_find(ns, \"%s\");\n", level, c->defined);
	_tabs(thiz, level);
	fprintf(thiz->out, "Ender_Value_Type tmp%d;\n", level + 1);
	_tabs(thiz, level);
	fprintf(thiz->out, "Ender_Constraint *tmp%d;\n", level + 2);
	_tabs(thiz, level);
	fprintf(thiz->out, "tmp%d = ender_namespace_descriptor_find(ns, \"%s\");\n", level, c->defined);
	_tabs(thiz, level);
	fprintf(thiz->out, "if (tmp%d)\n", level);
	_tabs(thiz, level + 1);
	fprintf(thiz->out, "if (ender_descriptor_type_value_type_to(ender_descriptor_type(tmp%d), &tmp%d))\n", level, level + 1);
	_tabs(thiz, level + 1);
	fprintf(thiz->out, "{\n");
	_tabs(thiz, level + 2);
	fprintf(thiz->out, "tmp%d = ender_constraint_descriptor_new(tmp%d);\n", level + 2, level);
	_tabs(thiz, level + 2);
	fprintf(thiz->out, "tmp%d = ender_container_new(tmp%d);\n", level - 1, level + 1);
	_tabs(thiz, level + 2);
	fprintf(thiz->out, "ender_container_constraint_set(tmp%d, tmp%d);\n", level -1, level + 2);
	_tabs(thiz, level + 1);
	fprintf(thiz->out, "}\n");
}

static void _dump_container_recursive(Ender_Generator *thiz,
		Ender_Parser_Container *c, int level)
{
	Ender_Parser_Container *sc;
	Eina_List *l;

	/* dump the main container variable */
	_tabs(thiz, level);
	fprintf(thiz->out, "Ender_Container *tmp%d = NULL;\n", level);
	_tabs(thiz, level);
	/* in case we have a name, we need to find it */
	if (c->defined)
	{
		fprintf(thiz->out, "{\n");
		_find_container(thiz, c, level + 1);
		_tabs(thiz, level);
		fprintf(thiz->out, "}\n");
	}
	/* in case we have a value type use it */
	else
	{
		fprintf(thiz->out, "tmp%d = ender_container_new(ENDER_%s);\n", level, ender_value_type_string_to(c->type));
	}
	/* now the subcontainers */
	EINA_LIST_FOREACH (c->subcontainers, l, sc)
	{
		_tabs(thiz, level + 1);
		fprintf(thiz->out, "{\n");
		_dump_container_recursive(thiz, sc, level + 2);
		_tabs(thiz, level + 1);
		fprintf(thiz->out, "}\n");
	}
}
/*----------------------------------------------------------------------------*
 *                            The parser interface                            *
 *----------------------------------------------------------------------------*/
static void _generator_add_namespace(void *data, const char *name, int version)
{
	Ender_Generator *thiz;

	thiz = data;
	thiz->ns_name = strdup(name);
	thiz->ns_version = version;
}

static void _generator_add_native(void *data, const char *name, Ender_Descriptor_Type type, const char *parent)
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
	/* the shutdown */
	fprintf(thiz->out, "static void _%s_%s_shutdown(void)\n", thiz->ns_name, thiz->name);
	fprintf(thiz->out, "{\n");
	/* TODO find the namespace
	 * get the descriptor
	 * unregister it
	 */
	fprintf(thiz->out, "}\n");
	/* the initialization */
	fprintf(thiz->out, "static void _%s_%s_init(void)\n", thiz->ns_name, thiz->name);
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
	if (type == ENDER_TYPE_ABSTRACT)
		fprintf(thiz->out, "\td = ender_namespace_descriptor_add(ns, \"%s\", NULL, ENDER_DESTRUCTOR(_%s_%s_delete), parent, ENDER_TYPE_%s);\n",
				thiz->name, thiz->ns_name, thiz->name,
				ender_descriptor_type_string_to(type));
	else
		fprintf(thiz->out, "\td = ender_namespace_descriptor_add(ns, \"%s\", ENDER_CREATOR(_%s_%s_new), ENDER_DESTRUCTOR(_%s_%s_delete), parent, ENDER_TYPE_%s);\n",
				thiz->name, thiz->ns_name, name, thiz->ns_name, name,
				ender_descriptor_type_string_to(type));
	fprintf(thiz->out, "\tif (!d) return;\n");
}

static void _generator_add_property(void *data, Ender_Parser_Property *p)
{
	Ender_Generator *thiz;
	Ender_Value_Type type;
	char pname[PATH_MAX];
	char pnormalized[PATH_MAX];

	thiz = data;
	if (!thiz->name_matched) return;

	/* generate the normalized name */
	_property_normalized(pnormalized, p->def.name);
	/* first the container */
	fprintf(thiz->out, "\t{\n");
	fprintf(thiz->out, "\t\tEnder_Container *ec;\n");
	fprintf(thiz->out, "\t\tEnder_Property *ep;\n");
	_dump_container_recursive(thiz, p->container, 2);
	fprintf(thiz->out, "\t\tec = tmp2;\n");
	fprintf(thiz->out, "\t\tep = ender_descriptor_property_add(d, \"%s\",\n", p->def.name);
	fprintf(thiz->out, "\t\t\t\tec,\n");
	fprintf(thiz->out, "\t\t\t\tENDER_GETTER(_%s_%s_%s_get),\n", thiz->ns_name, thiz->name, pnormalized);
	fprintf(thiz->out, "\t\t\t\tENDER_SETTER(_%s_%s_%s_set),\n", thiz->ns_name, thiz->name, pnormalized);
	if (p->container->type == ENDER_LIST)
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
	fprintf(thiz->out, "\t\t\t\t%s\n", p->rel ? "EINA_TRUE" : "EINA_FALSE");
	fprintf(thiz->out, "\t\t\t\t);\n");
	/* now assing the exported property variable to the recently created property */
	_property_variable_name(pname, thiz->ns_name, thiz->name, pnormalized);
	fprintf(thiz->out, "\t\t%s = ep;\n", pname, pname);
	/* close everything */
	fprintf(thiz->out, "\t}\n");
}

static void _generator_add_function(void *data, Ender_Parser_Function *f)
{
	Ender_Generator *thiz;
	Ender_Parser_Container *c;
	Ender_Value_Type type;
	Eina_List *l;
	char fname[PATH_MAX];
	char fnormalized[PATH_MAX];

	thiz = data;
	if (!thiz->name_matched) return;

	/* generate the normalized name */
	_property_normalized(fnormalized, f->def.name);
	/* first the container */
	fprintf(thiz->out, "\t{\n");
	fprintf(thiz->out, "\t\tEnder_Container *ret = NULL;\n");
	fprintf(thiz->out, "\t\tEina_List *args = NULL;\n");
	fprintf(thiz->out, "\t\tEnder_Function *ef;\n");
	if (f->ret)
	{
		fprintf(thiz->out, "\t\t{\n");
		_dump_container_recursive(thiz, f->ret, 3);
		fprintf(thiz->out, "\t\t\tret = tmp3;\n");
		fprintf(thiz->out, "\t\t}\n");
	}
	EINA_LIST_FOREACH (f->args, l, c)
	{
		fprintf(thiz->out, "\t\t{\n");
		_dump_container_recursive(thiz, c, 3);
		fprintf(thiz->out, "\t\t\targs = eina_list_append(args, tmp3);\n");
		fprintf(thiz->out, "\t\t}\n");
	}
	fprintf(thiz->out, "\t\tef = ender_descriptor_function_add_list(d, \"%s\",\n", f->def.alias ? f->def.alias : f->def.name);
	fprintf(thiz->out, "\t\t\t\tENDER_FUNCTION(_%s_%s_%s),\n", thiz->ns_name, thiz->name, fnormalized);
	fprintf(thiz->out, "\t\t\t\tNULL, ret, args);\n");
	/* now assing the exported function variable to the recently created function */
	_property_variable_name(fname, thiz->ns_name, thiz->name, fnormalized);
	fprintf(thiz->out, "\t\t%s = ef;\n", fname, fname);
	/* close everything */
	fprintf(thiz->out, "\t}\n");
}

static Ender_Parser_Descriptor _generator_parser = {
	/* .add_using 		= */ NULL,
	/* .add_namespace 	= */ _generator_add_namespace,
	/* .add_native 		= */ _generator_add_native,
	/* .add_property 	= */ _generator_add_property,
	/* .add_function 	= */ _generator_add_function,
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
	printf("NAME is the element name to dump\n");
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

