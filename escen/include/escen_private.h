#ifndef _ESCEN_PRIVATE_H
#define _ESCEN_PRIVATE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define ERR(...) EINA_LOG_DOM_ERR(escen_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(escen_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(escen_log_dom, __VA_ARGS__)
extern int escen_log_dom;

typedef struct _Escen_Parser Escen_Parser;
typedef struct _Escen_Scriptor Escen_Scriptor;

struct _Escen_Parser
{
	Escen *escen;
	Eina_Array *escen_ender;
	Eina_Array *escen_state;
	Eina_Array *escen_proxy;
	Escen_Animation *animation;
	char *file;
	char *ns;
};

struct _Escen_Scriptor
{
	void *(*load)(const char *script_text);
	Eina_Bool (*exec)(Ender_Element *, void *script_data);
};

Eina_Bool escen_script_neko_init(void);
void escen_script_neko_shutdown(void);

void escen_scriptor_register(Escen_Scriptor *es, const char *type);
void escen_scriptor_init(void);
void escen_scriptor_shutdown(void);
void * escen_scriptor_load(const char *script_text, const char *type, Escen_Scriptor **scriptor);
void escen_scriptor_exec(Escen_Scriptor *es, Ender_Element *e, void *data);

#if BUILD_SCRIPT_NEKO
extern Escen_Scriptor escen_scriptor_neko;
#endif

/* escen_main */
/* escen_animation */
void escen_animation_instance_set(Escen_Animation *ea, Escen_Instance *eei,
		Etch_Time time);

/* escen_copier */
void escen_copier_init(void);
Eina_Bool escen_copier_copy(Ender_Value_Type type,
		Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type);
/* escen ender */
Escen_Ender * escen_ender_new(Escen *escen, Ender_Descriptor *descriptor, const char *name);

/* escen_instance */
/* FIXME for now */
typedef struct _Escen_Animation_Instance
{
	Escen_Instance *eei;
	Escen_Animation *ea;
	Etch_Animation *eta;
	Ender_Property *property;
} Escen_Animation_Instance;

Etch * escen_instance_etch_get(Escen_Instance *ei);
void escen_instance_animation_instance_add(Escen_Instance *ei, Escen_Animation_Instance *eai);

/* escen_state */
Escen_State * escen_state_new(Escen_Ender *ee, const char *name);

/* escen_misc */
//Enesim_Argb enesim_color_lut_get(const char *name);
void escen_misc_init(void);

/* escen_script */
Escen_Script * escen_script_new(Escen_Scriptor *scriptor, void *data);

/* escen_setter */
void escen_setter_instance_set(Escen_Setter *set, Escen_Instance *eei);

#endif
