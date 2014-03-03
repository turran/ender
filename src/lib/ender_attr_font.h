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
#ifndef ENDER_ATTR_FONT_H_
#define ENDER_ATTR_FONT_H_

#define ENDER_ATTR_FONT_SET(f) ((Ender_Attr_Font_Set)(f))
#define ENDER_ATTR_FONT_GET(f) ((Ender_Attr_Font_Get)(f))

typedef void (*Ender_Attr_Font_Set)(void *o, Egueb_Css_Font *v);
typedef void (*Ender_Attr_Font_Get)(void *o, Egueb_Css_Font *v);

EAPI Egueb_Dom_Node * ender_attr_font_new(const char *name,
		Ender_Attr_Font_Get get, Ender_Attr_Font_Set set);

#endif

