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

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void help(void)
{
	printf("Usage: ender_loader FILE.ender\n");
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int main(int argc, char *argv[])
{
	Egueb_Dom_Node *doc = NULL;
	Enesim_Stream *stream;

	if (argc < 2)
	{
		help();
		return -1;
	}
	ender_init();

	stream = enesim_stream_file_new(argv[1], "r+");
	if (!stream)
	{
		printf("Fail to read %s\n", argv[1]);
		return -1;
	}

	/* create the document */
	doc = ender_document_new();
	egueb_dom_parser_parse(stream, doc);
	enesim_stream_unref(stream);

	if (doc)
		egueb_dom_node_unref(doc);

	ender_shutdown();
	return 0;
}

