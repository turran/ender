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
#ifndef _ENDER_H
#define _ENDER_H

#include <Eina.h>
#include <Enesim.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ENDER_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif
# else
#  define EAPI __declspec(dllimport)
# endif
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @mainpage Ender
 * @section intro Introduction
 * Ender is library written in C that describes other libraries. 
 * The purpose of Ender is to easily create bindings for any C library without
 * much struggling.
 *
 * @section dependencies Dependencies
 * - Eina
 */

#include "ender_main.h"
#include "ender_value.h"
#include "ender_item.h"
#include "ender_lib.h"
#include "ender_item_basic.h"
#include "ender_item_def.h"
#include "ender_item_struct.h"
#include "ender_item_attr.h"
#include "ender_item_object.h"
#include "ender_item_arg.h"
#include "ender_item_function.h"
#include "ender_item_constant.h"
#include "ender_item_enum.h"
#include "ender_utils.h"
#include "ender_parser.h"

#ifdef __cplusplus
}
#endif

#endif /*_ENDER_H*/
