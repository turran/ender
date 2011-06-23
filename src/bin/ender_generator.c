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

/* The idea here is to generate the .c code from an ender descriptor
 * file  (.ender). This way we can avoid the need to parse the file
 * everytime when ender initialize and register the different
 * namespaces statically
 */

static void help(void)
{
	printf("Run: ender_generator INPUT NAME OUTPUT\n");
	printf("Where INPUT is the ender file to load\n");
	printf("NAME is the element name to dump\n");
	printf("and OUTPUT is the c file to save\n");
}

int main(int argc, char **argv)
{
	FILE *out;

	ender_init(&argc, &argv);
	/* now parse the file input output */
	if (argc < 4)
	{
		help();
		return -1;
	}
	out = fopen(argv[3], "w");
	ender_generator_run(argv[1], argv[2], out);
	ender_shutdown();

	return 0;
}

