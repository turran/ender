/* ENDER - Enesim's descriptor library
 * Copyright (C) 2010 - 2012 Jorge Luis Zapata
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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Ender_Parser {
	const char *file;
	Ender_Parser_Descriptor *descriptor;
	void *data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_parser_add_using(Ender_Parser *thiz, Eina_List *using)
{
	Eina_List *l;
	char *str;

	if (!thiz->descriptor->add_using)
		return;
	EINA_LIST_FOREACH (using, l, str)
	{
		thiz->descriptor->add_using(thiz->data, str);
	}
}

void ender_parser_add_namespace(Ender_Parser *thiz, const char *name,
		int version)
{
	if (thiz->descriptor->add_namespace)
		thiz->descriptor->add_namespace(thiz->data, name, version);
}

void ender_parser_add_native(Ender_Parser *thiz, const char *name,
		const char *alias, Ender_Descriptor_Type type, const char *parent)
{
	if (thiz->descriptor->add_native)
		thiz->descriptor->add_native(thiz->data, name, alias, type, parent);
}

void ender_parser_add_property(Ender_Parser *thiz, Ender_Parser_Property *p)
{
	if (thiz->descriptor->add_property)
		thiz->descriptor->add_property(thiz->data, p);
}

void ender_parser_add_function(Ender_Parser *thiz, Ender_Parser_Function *f)
{
	if (thiz->descriptor->add_function)
		thiz->descriptor->add_function(thiz->data, f);
}

void ender_parser_container_free(Ender_Parser_Container *c)
{
	Ender_Parser_Container *s;
	EINA_LIST_FREE(c->subcontainers, s)
		ender_parser_container_free(s);
	if (c->defined)
		free(c->defined);
	free(c);
}

void ender_parser_error(Ender_Parser *thiz, const char *str, int line,
	int col)
{
	ERR("Parsing error at %s %d: %d %s", thiz->file, line, col, str);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_parser_parse(const char *file,
		Ender_Parser_Descriptor *descriptor, void *data)
{
	Ender_Parser thiz;
	void *scanner;
	FILE *f;
	int ret;

	f = fopen(file, "r");
	if (!f) return EINA_FALSE;

	thiz.descriptor = descriptor;
	thiz.file = file;
	thiz.data = data;

	ender_lex_init(&scanner);
	ender_set_in(f, scanner);
	ret = ender_parse(scanner, &thiz);

	ender_lex_destroy(scanner);
	fclose(f);

	return !ret;
}

#if 0
/**
 *
 */
EAPI const char * ender_parser_namepsace_reduce(const char *ns)
{

}

/**
 *
 */
EAPI const char * ender_parser_locate_file(const char *name)
{

}
#endif
