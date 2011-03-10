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
#include <getopt.h>

static void _list_cb(const char *name, void *data)
{
	printf("%s\n", name);
}

static void _prop_cb(Ender *e, const char *name, void *data)
{
	Ender_Property *p;

	p = ender_property_get(e, name);
	// ender_property_is_relative(p);
	printf("- %s %d\n", name, ender_property_type(p));
}

int main(int argc, char **argv)
{
	/* if no argument is passed, get every ender and list it */
	/* if we have an ender name, first try to match a renderer
	 * then an abstract and then a namespace
	 */
	ender_init();
	if (argc < 2)
	{
		ender_list(_list_cb, NULL);
	}
	else
	{
		Ender *e;

		e = ender_new(argv[1]);
		if (!e) goto end;
		ender_property_list(e, _prop_cb, NULL);
		/* create an ender for the given name, iterate over the
		 * properties, get the descriptions, etc
	 	*/
	}
end:
	ender_shutdown();
	return 0;
}
