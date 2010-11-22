#ifndef _ENDER_PRIVATE_H
#define _ENDER_PRIVATE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dlfcn.h>

/* core */
typedef struct _Ender_Descriptor Ender_Descriptor;

typedef Enesim_Renderer * (*Ender_Creator)(void);
typedef void (*Ender_Getter)(Enesim_Renderer *r, ...);
typedef void (*Ender_Setter)(Enesim_Renderer *r, ...);

Ender_Descriptor * ender_descriptor_register(const char *name, Ender_Creator creator,
		Ender_Descriptor *parent);
void ender_descriptor_unregister(Ender_Descriptor *edesc);
const char * ender_descriptor_name_get(Ender_Descriptor *edesc);
Ender_Descriptor * ender_descriptor_get(const char *name);
void ender_descriptor_property_add(Ender_Descriptor *edesc, const char *name, Ender_Property_Type t,
	Ender_Getter get, Ender_Setter set);

/* the parser */
void ender_parser_init(void);
void ender_parser_shutdown(void);
void ender_parser_parse(const char *file);

#endif
