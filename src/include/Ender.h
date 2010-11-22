#ifndef _ENDER_H
#define _ENDER_H

#include "Eina.h"
#include "Enesim.h"

/**
 * @mainpage Ender
 * @section intro Introduction
 *
 * @section dependencies Dependencies
 * - Eina
 * - Enesim
 *
 * @file
 * @brief Ender API
 * @defgroup Ender_Group API
 * @{
 *
 * @todo
 */

typedef struct _Ender Ender;

typedef enum _Ender_Property_Type
{
	ENDER_UINT32,
	ENDER_INT32,
	ENDER_FLOAT,
	ENDER_DOUBLE,
	ENDER_COLOR,
	ENDER_STRING,
	ENDER_MATRIX,
	ENDER_VALUES,
} Ender_Property_Type;


EAPI void ender_init(void);
EAPI void ender_shutdown(void);

EAPI Ender * ender_new(const char *name);
EAPI const char * ender_name_get(Ender *e);
EAPI Eina_Bool ender_property_get(Ender *e, char *name, Ender_Property_Type *type);

EAPI void ender_value_get(Ender *e, ...);
EAPI void ender_value_set(Ender *e, ...);
EAPI Enesim_Renderer * ender_renderer_get(Ender *e);

/**
 * @}
 */

#endif /*_ENDER_H*/
