/* ESCEN - Ender's based scene loader
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
#include "Escen.h"
#include "escen_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen * escen_parser_load(const char *file)
{
	Escen_Parser *parser;
	Escen *escen;
	void *scanner;
	FILE *f;
	int ret;
	char path[PATH_MAX];
	char *c;

	f = fopen(file, "r");
	if (!f) return NULL;

	parser = calloc(1, sizeof(Escen_Parser));
	parser->escen_ender = eina_array_new(1);
	parser->escen_state = eina_array_new(1);
	parser->escen_proxy = eina_array_new(1);
	parser->file = strdup(file);

	escen_lex_init(&scanner);
	escen_set_in(f, scanner);
	ret = escen_parse(scanner, parser);
	escen_lex_destroy(scanner);
	fclose(f);

	escen = parser->escen;
	eina_array_free(parser->escen_ender);
	eina_array_free(parser->escen_state);
	eina_array_free(parser->escen_proxy);
	free(parser->file);
	free(parser);

	/* add the realpath to the escen surfaces path */
	if (!realpath(file, path)) goto end;
	for (c = path + strlen(path); c != path;  c--)
	{
		if (*c == '/')
		{
			*c = '\0';
			break;
		}
	}
	escen_surface_path_append(escen, path);
end:
	return escen;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_parser_save(Escen *e, const char *file)
{
	/* iterate over the list of escen enders with no
 	 * parent, then get all the states and then
 	 * the setters, animations, scripts
 	 */
}
