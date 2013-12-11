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

#ifndef _ENDER_MARSHALLER_H
#define _ENDER_MARSHALLER_H

/**
 * @defgroup Ender_Function_Group Function
 * @{
 */

#define ENDER_FUNCTION(f) ((Ender_Accessor)(f))

typedef struct _Ender_Function Ender_Function;

EAPI int ender_function_args_count(Ender_Function *thiz);
EAPI const Eina_List * ender_function_args_get(Ender_Function *thiz);
EAPI Ender_Container * ender_function_ret_get(Ender_Function *thiz);
EAPI const char * ender_function_name_get(Ender_Function *thiz);

/**
 * @}
 */

