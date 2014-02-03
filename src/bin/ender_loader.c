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
	printf("Usage: ender_loader FILE.ender ID STATE\n");
	printf("Where:\n");
	printf("  ID is the id of the object you want to instantiate\n");
	printf("  STATE the state you want to set\n");
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int main(int argc, char *argv[])
{
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_Node *instance;
	Enesim_Stream *stream;
	Eina_Error err = 0;

	if (argc < 4)
	{
		help();
		return -1;
	}
	ender_init();

	stream = enesim_stream_file_new(argv[1], "r+");
	if (!stream)
	{
		printf("Fail to read %s\n", argv[1]);
		goto failed_file;
	}

	/* create the document */
	doc = ender_document_new();
	egueb_dom_parser_parse(stream, doc);
	enesim_stream_unref(stream);

	instance = ender_document_instance_new(doc, argv[2], &err);
	if (!instance)
	{
		printf("Instance '%s' not found (%s)\n", argv[2],
				err ? eina_error_msg_get(err): "unknown");
		goto failed_instance;
	}
	if (!ender_element_instance_state_set(instance, argv[3], &err))
	{
		printf("State '%s' setting failed (%s)\n", argv[3],
				err ? eina_error_msg_get(err): "unknown");
		goto failed_state;
	}
	/* TODO still think how the animation interface should be:
	 * 1. one etch per instance?
	 * 2. one etch per scene?
	 * 3. one etch per doc?
	 */
	//ender_document_tick(doc);
	egueb_dom_document_process(doc);

failed_state:	
	egueb_dom_node_unref(instance);

failed_instance:
	if (doc)
		egueb_dom_node_unref(doc);
failed_file:
	ender_shutdown();
	return 0;
}

