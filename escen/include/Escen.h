#ifndef _ESCEN_H
#define _ESCEN_H

#include <Eina.h>
#include <Enesim.h>
#include <Etch.h>
#include <Emage.h>
#include <Ender.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_ESCEN_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_ESCEN_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

/**
 * @mainpage Escen
 * @section intro Introduction
 *
 * @section dependencies Dependencies
 * - Eina
 * - Enesim
 *
 * @file
 * @brief Escen API
 *
 * @todo
 */

typedef struct _Escen Escen;
typedef Etch_Time Escen_Time;
typedef struct _Escen_Ender Escen_Ender;
typedef struct _Escen_Instance Escen_Instance;
typedef struct _Escen_State Escen_State;
typedef struct _Escen_Proxy Escen_Proxy;
typedef struct _Escen_Setter Escen_Setter;
typedef struct _Escen_Animation Escen_Animation;
typedef struct _Escen_Animation_Key Escen_Animation_Key;
typedef struct _Escen_Script Escen_Script;

typedef Enesim_Surface * (*Escen_Surface_Loader)(const char *file);
typedef Enesim_Surface * (*Escen_Surface_Creator)(int w, int h);
typedef void (*Escen_Instance_New_Callback)(Escen_Instance *eei, void *data);
/**
 * @defgroup Escen_Core Core
 * @{
 */
EAPI void escen_init(void);
EAPI void escen_shutdown(void);
EAPI void escen_version(unsigned int *major, unsigned int *minor, unsigned int *micro);

EAPI Escen * escen_new(void);
EAPI void escen_delete(Escen *e);
EAPI void escen_surface_path_append(Escen *e, const char *path);
EAPI void escen_surface_loader_set(Escen *e, Escen_Surface_Loader loader);
EAPI void escen_surface_creator_set(Escen *e, Escen_Surface_Creator creator);
EAPI Enesim_Surface * escen_surface_default_loader(const char *file);
EAPI Enesim_Surface * escen_surface_default_creator(int w, int h);

EAPI void escen_fps_set(Escen *e, unsigned int fps);
EAPI unsigned int escen_fps_get(Escen *e);

EAPI Escen * escen_parser_load(const char *file);
EAPI void escen_parser_save(Escen * escen, const char *file);

/**
 * @}
 * @defgroup Escen_Ender Enders
 * @{
 */
EAPI int escen_ender_count(Escen *e);
EAPI Escen_Ender * escen_ender_add(Escen *e, Ender_Descriptor *descriptor, const char *name);
EAPI Escen_State * escen_ender_state_alias_add(Escen_Ender *ee, const char *name, const char *alias);
EAPI Escen_Ender * escen_ender_get(Escen *e, const char *name);
EAPI Escen_Ender * escen_ender_nth_get(Escen *e, int nth);
EAPI Escen * escen_ender_escen_get(Escen_Ender *ee);
EAPI const char * escen_ender_name_get(Escen_Ender *e);
EAPI Ender_Descriptor * escen_ender_descriptor_get(Escen_Ender *ee);
/**
 * @}
 * @defgroup Escen_Instance Instance
 * @{
 */
EAPI void escen_instance_new_callback_add(Escen_Instance_New_Callback cb, void *data);
EAPI Escen_Instance * escen_instance_new(Escen_Ender *e, Eina_Bool set_default);
EAPI void escen_instance_delete(Escen_Instance *eei);
EAPI void escen_instance_state_set(Escen_Instance *eei, Escen_State *es);
EAPI Ender_Element * escen_instance_ender_get(Escen_Instance *ei);
EAPI Escen * escen_instance_escen_get(Escen_Instance *eei);
EAPI Escen_Ender * escen_instance_escen_ender_get(Escen_Instance *e);
EAPI void escen_instance_process(Escen_Instance *eei);
EAPI Escen_State * escen_instance_current_state_get(Escen_Instance *ei);
EAPI Eina_Bool escen_instance_current_state_finalized(Escen_Instance * ei);

/**
 * @}
 * @defgroup Escen_Proxy Proxy
 * @{
 */
EAPI Escen_Ender * escen_proxy_ender_get(Escen_Proxy *ep);
EAPI const char * escen_proxy_name_get(Escen_Proxy *ep);
EAPI const char * escen_proxy_destination_name_get(Escen_Proxy *ep);
EAPI void escen_proxy_setter_add(Escen_Proxy *es, Escen_Setter *et);
EAPI void escen_proxy_animation_add(Escen_Proxy *es, Escen_Animation *ea);

/**
 * @}
 * @defgroup Escen_State States
 * @{
 */

EAPI Escen_State * escen_ender_state_add(Escen_Ender *ee, const char *name);
EAPI Escen_State * escen_ender_state_get(Escen_Ender *ee, const char *name);
EAPI int escen_ender_state_count(Escen_Ender *ee);
EAPI Escen_State * escen_ender_state_nth_get(Escen_Ender *ee, int nth);
EAPI const char * escen_state_name_get(Escen_State *es);
EAPI Escen_Ender * escen_state_ender_get(Escen_State *es);

EAPI void escen_state_setter_add(Escen_State *es, Escen_Setter *s);
EAPI void escen_state_animation_add(Escen_State *es, Escen_Animation *ea);

/**
 * @}
 * @defgroup Escen_Animation Animations
 * @{
 */
EAPI Escen_Animation * escen_animation_new(const char *name, Eina_Bool wait);
EAPI Escen_Animation_Key * escen_animation_key_add(Escen_Animation *ea, Ender_Value *value, Escen_Time time, Etch_Interpolator_Type type);
EAPI Eina_Bool escen_animation_state_waits(Escen_Animation *ea);
/**
 * @}
 * @defgroup Escen_Setter Setters
 * @{
 */
EAPI Escen_Setter * escen_setter_new(const char *name, Ender_Value *v);
/**
 * @}
 * @defgroup Escen_Script Scripts
 * @{
 */
EAPI Escen_Script * escen_state_script_add(Escen_State *es, const char *type, const char * script);
/**
 * @}
 * @}
 */

#endif /*_ESCEN_H*/
