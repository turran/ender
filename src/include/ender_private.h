#ifndef _ENDER_PRIVATE_H
#define _ENDER_PRIVATE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dlfcn.h>

/* core */
#define ERR(...) EINA_LOG_DOM_ERR(ender_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(ender_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(ender_log_dom, __VA_ARGS__)
extern int ender_log_dom;

typedef struct _Ender_Descriptor Ender_Descriptor;
typedef struct _Ender_Descriptor_Property Ender_Descriptor_Property;

Ender_Property * ender_property_new(Ender_Property_Type t);
void ender_property_delete(Ender_Property *p);
void ender_property_add(Ender_Property *p, Ender_Property *sub);

typedef Enesim_Renderer * (*Ender_Creator)(void);
typedef void (*Ender_Getter)(Enesim_Renderer *r, ...);
typedef void (*Ender_Setter)(Enesim_Renderer *r, ...);
typedef void (*Ender_Add)(Enesim_Renderer *r, ...);
typedef void (*Ender_Remove)(Enesim_Renderer *r, ...);
typedef void (*Ender_Clear)(Enesim_Renderer *r);
typedef void (*Ender_Init)(void);
typedef void (*Ender_Shutdown)(void);

Ender_Descriptor * ender_descriptor_register(const char *name, Ender_Creator creator,
		Ender_Descriptor *parent);
void ender_descriptor_unregister(Ender_Descriptor *edesc);
const char * ender_descriptor_name_get(Ender_Descriptor *edesc);
Ender_Descriptor * ender_descriptor_get(const char *name);
void ender_descriptor_property_add(Ender_Descriptor *edesc, const char *name, Ender_Property *p,
	Ender_Getter get, Ender_Setter set,
	Ender_Add add, Ender_Remove remove, Ender_Clear clear,
	Eina_Bool relative);

/* the parser */
typedef struct _Ender_Parser_Property Ender_Parser_Property;

struct _Ender_Parser_Property
{
	char *name;
	Eina_Bool relative;
	Eina_Array *properties;
};

Ender_Descriptor * ender_parser_register(const char *ns, const char *name, Ender_Descriptor * parent);
void ender_parser_init(void);
void ender_parser_shutdown(void);
void ender_parser_parse(const char *file);

#endif
