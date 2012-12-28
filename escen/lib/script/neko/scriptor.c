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
#include "neko_vm.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Escen_Neko
{
	value loader;
	value ender_init;
} Escen_Neko;

typedef struct _Escen_Neko_Script
{
	char *name;
	value loaded;
} Escen_Neko_Script;

static Escen_Neko _escen_neko;
static int _init = 0;

static value _load_bindings(void)
{
	value args[2];
	value exc = NULL;
	value ret;
	value loadprim;

	loadprim = val_field(_escen_neko.loader, val_id("loadprim"));
	args[0] = alloc_string("ender@element_initialize");
	args[1] = alloc_int(1);
	ret = val_callEx(_escen_neko.loader, loadprim, args, 2, &exc);
	if (exc)
	{

		buffer b = alloc_buffer(NULL);
		val_buffer(b, exc);
		printf("Uncaught exception - %s\n", val_string(buffer_to_string(b)));
		return NULL;
	}
	return ret;
}

static value _load_compiled(const char *name)
{
	value args[2];
	value exc = NULL;
	value ret;
	value loadmodule;

	args[0] = alloc_string(name);
	args[1] = _escen_neko.loader;

	loadmodule = val_field(_escen_neko.loader, val_id("loadmodule"));
	ret = val_callEx(_escen_neko.loader, loadmodule, args, 2, &exc);
	if (exc != NULL)
	{
		buffer b = alloc_buffer(NULL);
		val_buffer(b, exc);
		printf("Uncaught exception - %s\n", val_string(buffer_to_string(b)));
		return NULL;
	}
	return ret;
}

static void _execute(value module, value neko_element)
{
	value f;
	value ret;

	f = val_field(module, val_id("f"));
	ret = val_call1(f, neko_element);
}
/*----------------------------------------------------------------------------*
 *                          The scriptor interface                            *
 *----------------------------------------------------------------------------*/
static void * _neko_load(const char *script_text)
{
	Escen_Neko_Script *ens;
	FILE *source;
	char dst[PATH_MAX];
	char *name;
	pid_t pid;
	value loaded;
	const char header[] = "$exports.f = function(e) {";
	const char footer[] = "};";

	name = tmpnam(NULL);
	if (!name) return NULL;

	source = fopen(name, "w");
	/* lets wrap our code */	
	fwrite(header, strlen(header), sizeof(char), source);
	fwrite(script_text, strlen(script_text), sizeof(char), source);
	fwrite(footer, strlen(footer), sizeof(char), source);
	/* given that we dont know yet how to instantiate a jit
	 * VM, fork the compiler to compile this file
	 */
	fclose(source);
	pid = fork();
	if (pid == 0)
	{
		if (execlp("nekoc", "nekoc", name, NULL) < 0)
		{
			perror("failed");
			printf("error\n");
		}
	}
	else if (pid < 0)
	{
		printf("Error while forking\n");
		return NULL;
	}
	/* wait for child to finish */
	waitpid(pid, NULL, 0);

	/* append to name an ending .n of the compiled source */
	snprintf(dst, PATH_MAX, "%s.n", name);
	printf("source %s compiled %s\n", name, dst);
	loaded = _load_compiled(dst);
	if (!loaded)
	{
		printf("failed loading the compiled file\n");
		return NULL;
	}

	ens = calloc(1, sizeof(Escen_Neko_Script));
	ens->name = strdup(dst);
	ens->loaded = loaded;

	return ens;
}

static Eina_Bool _neko_exec(Ender_Element *e, void *script_data)
{
	Escen_Neko_Script *ens = script_data;
	vkind k_element = NULL;
	value neko_element;
	value ret;

	/* the script data already has the compiled bytecode
	 * just convert the ender into a neko value and call
	 * the function as if the first parameter would be
	 * the ender
	 */
	kind_share(&k_element, "ender_element");
	neko_element = alloc_abstract(k_element, e);
	ret = val_call1(_escen_neko.ender_init, neko_element);
	if (ret == val_null)
	{
		printf("failed executing\n");
		return EINA_FALSE;
	}
	printf("trying to exec a script on %p\n", e);
	_execute(ens->loaded, ret);
	printf("file executed correctly\n");

	return EINA_TRUE;
}

Escen_Scriptor escen_scriptor_neko = {
	.load = _neko_load,
	.exec = _neko_exec,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool escen_script_neko_init(void)
{
	neko_vm *vm;
	void *loader;

	if (_init) return EINA_TRUE;

	/* instantiate a VM */
	neko_global_init(NULL);
	vm = neko_vm_alloc(NULL);
	neko_vm_select(vm);

	loader = neko_default_loader(NULL, 0);
	if (!loader) return EINA_FALSE;

	_escen_neko.loader = loader;
	/* load the bindings */
	printf("loading the bindings\n");
	_escen_neko.ender_init = _load_bindings();
	if (!_escen_neko.ender_init) return EINA_FALSE;

	/* TODO check for existance of the compiler */
	/* FIXME given that we cannot send the output of the neko
	 * compiler to a user defined file, we can send to a different
	 * output and then read that file
	 */
	_init = EINA_TRUE;
}

void escen_script_neko_shutdown(void)
{
	if (!_init) return;

	neko_global_free();
	_init = EINA_FALSE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
