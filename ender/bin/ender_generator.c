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
#include <unistd.h>
#include <getopt.h>

#include "Ender.h"
#include "ender_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define F(f, level, format, ...) \
	_tabs(f, level); \
	fprintf(f, format "\n", __VA_ARGS__);

static Eina_Bool use_extended = EINA_FALSE;
static Eina_Bool use_offsetof = EINA_TRUE;
static Eina_Bool use_sizeof = EINA_TRUE;

typedef struct _Ender_Generator
{
	char *ns_name;
	int ns_version;
	const char *name;
	FILE *out;
	Eina_Bool found;
	Ender_Descriptor_Type descriptor_type;
	Eina_Bool name_matched;
	Eina_Bool extended;
	Eina_Bool container_initialized;
} Ender_Generator;

static void _tabs(FILE *out, int count)
{
	int i;

	for (i = 0; i < count; i++)
		fprintf(out, "\t");
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

#if 0
	printf("%s: %s %s %s\n", __FUNCTION__, ns, oname, pname);
#endif

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
	F(thiz->out, level, "Ender_Descriptor *tmp%d = ender_namespace_descriptor_find(ns, \"%s\");", level, c->defined);
	F(thiz->out, level, "Ender_Value_Type tmp%d;", level + 1);
	F(thiz->out, level, "Ender_Constraint *tmp%d;\n", level + 2);
	F(thiz->out, level, "tmp%d = ender_namespace_descriptor_find(ns, \"%s\");", level, c->defined);
	F(thiz->out, level, "if (tmp%d)", level);
	F(thiz->out, level + 1, "if (ender_descriptor_type_value_type_to(ender_descriptor_type(tmp%d), &tmp%d))", level, level + 1);
	F(thiz->out, level + 1, "{", NULL);
	F(thiz->out, level + 2, "tmp%d = ender_constraint_descriptor_new(tmp%d);", level + 2, level);
	F(thiz->out, level + 2, "tmp%d = ender_container_new(tmp%d);", level - 1, level + 1);
	F(thiz->out, level + 2, "ender_container_constraint_set(tmp%d, tmp%d);", level -1, level + 2);
	F(thiz->out, level + 1, "}", NULL);
}

static void _dump_container_recursive(Ender_Generator *thiz,
		Ender_Parser_Container *c, int level)
{
	Ender_Parser_Container *sc;
	Eina_List *l;

	/* dump the main container variable */
	F(thiz->out, level, "Ender_Container *tmp%d = NULL;", level);
	/* in case we have a name, we need to find it */
	if (c->defined)
	{
		F(thiz->out, level, "{", NULL);
		_find_container(thiz, c, level + 1);
		F(thiz->out, level, "}", NULL);
	}
	/* in case we have a value type use it */
	else
	{
		F(thiz->out, level, "tmp%d = ender_container_new(ENDER_%s);", level, ender_value_type_string_to(c->type));
	}
	/* now the subcontainers */
	EINA_LIST_FOREACH (c->subcontainers, l, sc)
	{
		F(thiz->out, level + 1, "{", NULL);
		_dump_container_recursive(thiz, sc, level + 2);
		F(thiz->out, level + 1, "}", NULL);
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

static void _generator_add_native(void *data, const char *name, const char *alias,
		Ender_Descriptor_Type type, const char *parent)
{
	Ender_Generator *thiz;
	const char *real_creator = NULL;
	const char *real_size = NULL;
	char creator[PATH_MAX] = { 0 };
	char size[PATH_MAX] = { 0 };

	thiz = data;
	if (strcmp(name, thiz->name))
	{
		thiz->name_matched = EINA_FALSE;
		return;
	}
	thiz->name_matched = EINA_TRUE;
	thiz->found = EINA_TRUE;
	thiz->descriptor_type = type;

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
	if (type != ENDER_TYPE_ABSTRACT)
	{
		sprintf(creator, "_%s_%s_new", thiz->ns_name, thiz->name);
		real_creator = creator;

		if (type == ENDER_TYPE_STRUCT)
		{
			char *cns_name = ender_name_structify(thiz->ns_name);
			char *cname = ender_name_structify(name);
			sprintf(size, "sizeof(%s_%s)", cns_name, cname);
			real_size = size;
			free (cns_name);
			free (cname);
		}
	}
	fprintf(thiz->out, "\td = ender_namespace_descriptor_add(ns, \"%s\", "
			"ENDER_CREATOR(%s), ENDER_DESTRUCTOR(_%s_%s_delete), "
			"parent, ENDER_TYPE_%s, %s);\n",
			alias ? alias : name,
			real_creator ? real_creator : "NULL",
			thiz->ns_name, name,
			ender_descriptor_type_string_to(type),
			real_size ? real_size : "-1");
	fprintf(thiz->out, "\tif (!d) return;\n");
}

static void _generator_add_property(void *data, Ender_Parser_Property *p)
{
	Ender_Generator *thiz;
	Ender_Value_Type type;
	char pname[PATH_MAX];
	char pnormalized[PATH_MAX];
	char getter[PATH_MAX];
	char setter[PATH_MAX];
	char offset[PATH_MAX];
	char *real_offset = NULL;

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
	sprintf(getter, "_%s_%s_%s_get", thiz->ns_name, thiz->name, pnormalized);
	sprintf(setter, "_%s_%s_%s_set", thiz->ns_name, thiz->name, pnormalized);
	fprintf(thiz->out, "\t\t\t\tENDER_GETTER(%s),\n", getter);
	fprintf(thiz->out, "\t\t\t\tENDER_SETTER(%s),\n", setter);
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
	if (use_extended)
		fprintf(thiz->out, "\t\t\t\tENDER_IS_SET(_%s_%s_%s_is_set),\n", thiz->ns_name, thiz->name, pnormalized);
	else
		fprintf(thiz->out, "\t\t\t\tNULL,\n");
	fprintf(thiz->out, "\t\t\t\t%s,\n", p->rel ? "EINA_TRUE" : "EINA_FALSE");
	if (thiz->descriptor_type == ENDER_TYPE_STRUCT && use_offsetof)
	{
		char *cns_name = ender_name_structify(thiz->ns_name);
		char *cname = ender_name_structify(thiz->name);

		sprintf(offset, "offsetof(%s_%s, %s)", cns_name, cname, pnormalized);
		real_offset = offset;
	}
	fprintf(thiz->out, "\t\t\t\t%s);\n", real_offset ? real_offset : "-1");
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

static void _generator_run(const char *i, const char *element, const char *o)
{
	Ender_Generator thiz;
	FILE *out;

	out = fopen(o, "w");

	thiz.name = element;
	thiz.out = out;
	thiz.found = EINA_FALSE;
	thiz.name_matched = EINA_FALSE;
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
	printf("  s Do not use sizeof to define structs sizes\n");
	printf("  o Do not use offsetof to define structs property offsets\n");
	printf("INPUT is the ender file to load\n");
	printf("NAME is the element name to dump\n");
	printf("and OUTPUT is the c file to save\n");
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int main(int argc, char **argv)
{
	char *in;
	char *out;
	char *name;
	char *short_options = "xsoh";
	struct option long_options[] = {
		{"extended", 0, 0, 'x'},
		{"nosizeof", 0, 0, 's'},
		{"nooffsetof", 0, 0, 'o'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	int c;
	int option;

	/* handle the parameters */
	while ((c = getopt_long(argc, argv, short_options, long_options,
			&option)) != -1)
	{
		switch (c)
		{
			case 'h':
			help();
			return 0;

			case 'x':
			use_extended = EINA_TRUE;
			break;

			case 's':
			use_sizeof = EINA_FALSE;
			break;

			case 'o':
			use_offsetof = EINA_FALSE;
			break;

			default:
			break;
		}
	}

	if (optind >= argc)
	{
		help();
		return 0;
	}

	if (argc - optind < 3)
	{
		help();
		return 0;
	}

	in = argv[argc - 3];
	name = argv[argc - 2];
	out = argv[argc - 1];

	ender_init();
	_generator_run(in, name, out);
	ender_shutdown();

	return 0;
}

