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
 * everytime whene ender initialize and register the different
 * namespaces statically
 */

static void help(void)
{
	printf("Run: ender_generator INPUT OUTPUT\n");
	printf("Where INPUT is the ender file to load\n");
	printf("and OUTPUT is the c file to save\n");
}

int main(int argc, char **argv)
{
	int myargc;
	char **myargv;

	/* append the disable-parse option */
	myargc = argc + 1;
	myargv = malloc(sizeof(char *) * myargc);
	memcpy(myargv, argv, argc * sizeof(char *));
	myargv[myargc - 1] = strdup("--disable-parse");
	/* we force to disable the parse */
	ender_init(&myargc, &myargv);
	/* now parse the file input output */
	if (argc < 3)
	{
		help();
		return -1;
	}
	ender_parser_load(argv[1]);

	ender_shutdown();

	return 0;
}

