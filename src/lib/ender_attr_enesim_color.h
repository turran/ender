/* ENDER - Enesim's descriptor library
 * Copyright (C) 2010 - 2012 Jorge Luis Zapata
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
#ifndef ENDER_ATTR_ENESIM_COLOR_H_
#define ENDER_ATTR_ENESIM_COLOR_H_

#define ENDER_ATTR_ENESIM_COLOR_SET(f) ((Ender_Attr_Enesim_Color_Set)(f))
#define ENDER_ATTR_ENESIM_COLOR_GET(f) ((Ender_Attr_Enesim_Color_Get)(f))

typedef void (*Ender_Attr_Enesim_Color_Set)(void *o, Enesim_Color v);
typedef Enesim_Color (*Ender_Attr_Enesim_Color_Get)(void *o);

EAPI Egueb_Dom_Node * ender_attr_enesim_color_new(const char *name,
		Ender_Attr_Enesim_Color_Get get, Ender_Attr_Enesim_Color_Set set);

#endif
