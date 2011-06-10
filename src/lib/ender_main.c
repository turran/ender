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

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _init = 0;
static Eina_Bool _enable_parse = EINA_TRUE;

/* TODO something to do on the future */
#if 0
typedef void (*Ender_Option_Callback)(Ender_Option *eo, char *arg);
typedef struct _Ender_Option
{
	const char *short_name;
	const char *long_name;
	const char *description;
	Eina_Bool needs_argument;
	Ender_Option_Callback callback;	
} Ender_Option;
#endif

static void _remove_option(int idx, int *argc, char ***argv)
{
	while (idx < *argc - 1)
	{
		(*argv)[idx] = (*argv)[idx+1];
		idx++;
	}
	*argc = *argc - 1;
}

static void _parse(int *argc, char ***argv)
{
	int i;
	char **option;

	if (!argc || !argv) return;
	option = *argv;
	for (i = 0; i < *argc; i++)
	{
		if (!strcmp("--disable-parse", option[i]))
		{
			_remove_option(i, argc, argv);
			_enable_parse = EINA_FALSE;
			i--;
		}
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int ender_log_dom = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Initialize the ender library
 */
EAPI void ender_init(int *argc, char ***argv)
{
	if (!_init++)
	{
		eina_init();
		_parse(argc, argv);
		ender_log_dom = eina_log_domain_register("ender", NULL);
		enesim_init();
		ender_descriptor_init();
		ender_namespace_init();
		ender_container_init();
		ender_parser_init();
		if (_enable_parse)
			ender_parser_parse();
	}
}

/**
 * Shutdown the ender library
 */
EAPI void ender_shutdown(void)
{
	if (_init == 1)
	{
		ender_parser_shutdown();
		ender_container_shutdown();
		ender_namespace_shutdown();
		ender_descriptor_shutdown();
		enesim_shutdown();
		eina_log_domain_unregister(ender_log_dom);
		eina_shutdown();
	}
	_init--;
}
