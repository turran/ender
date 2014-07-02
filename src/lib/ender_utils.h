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

#ifndef _ENDER_UTILS_H_
#define _ENDER_UTILS_H_

EAPI char * ender_utils_name_convert(const char *s, Ender_Case src_case,
		Ender_Notation src_not, Ender_Case dst, Ender_Notation dst_not);
EAPI char * ender_utils_to_upper(const char *s);
EAPI char * ender_utils_to_lower(const char *s);

#endif
