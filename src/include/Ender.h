#ifndef _ENDER_H
#define _ENDER_H

#include "Eina.h"
#include "Enesim.h"

/**
 * @mainpage Ender
 * @section intro Introduction
 * Ender is a library that loads descriptions of enesim renderers and abstracts
 * them into a collection of properties that are accesible through a common API.
 *
 * @section syntax Syntax
 * The syntax of a description file (.ender) is:
 * @code
 * namespace [PREFIX.]NAME {
 *   [ABSTRACT | CLASS] NAME {
 *     [float | color | renderer | unsigned int] NAME;
 *   }
 * }
 * @endcode
 *
 * @section examples Examples
 *
 * @section dependencies Dependencies
 * - Eina
 * - Enesim
 *
 * @defgroup Ender_Group API
 * @{
 *
 * @todo
 */

typedef struct _Ender Ender;
typedef struct _Ender_Property Ender_Property;

typedef enum _Ender_Property_Type
{
	ENDER_UINT32,
	ENDER_INT32,
	ENDER_FLOAT,
	ENDER_DOUBLE,
	ENDER_COLOR,
	ENDER_STRING,
	ENDER_MATRIX,
	ENDER_RENDERER,
	ENDER_LIST,
	ENDER_VALUES,
} Ender_Property_Type;


EAPI void ender_init(void);
EAPI void ender_shutdown(void);

EAPI Ender * ender_new(const char *name);
EAPI void ender_delete(Ender *e);
EAPI const char * ender_name_get(Ender *e);

EAPI Ender_Property * ender_property_get(Ender *e, char *name);
EAPI Ender_Property_Type ender_property_type(Ender_Property *p);
EAPI const Eina_Array * ender_property_sub(Ender_Property *p);

EAPI void ender_value_get(Ender *e, ...);
EAPI void ender_value_set(Ender *e, ...);
EAPI Enesim_Renderer * ender_renderer_get(Ender *e);

/**
 * @}
 */

#endif /*_ENDER_H*/
