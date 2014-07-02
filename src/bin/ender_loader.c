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
#include "Enesim.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void help(void)
{
	printf("Run: ender_loader FILE.ender\n");
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int main(int argc, char **argv)
{
	Enesim_Stream *s;

	if (argc < 2)
	{
		help();
		goto no_file;
	}

	ender_init();
	s = enesim_stream_file_new(argv[1], "r");
	if (!s) goto no_file;

	ender_parser_parse(s);
no_file:
	ender_shutdown();
	return 0;
}
