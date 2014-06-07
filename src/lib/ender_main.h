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

#ifndef _ENDER_MAIN_H_
#define _ENDER_MAIN_H_

/*
 * @defgroup Ender_Main_Group Main
 * @{
 */
EAPI void ender_init(void);
EAPI void ender_shutdown(void);
EAPI void ender_version(unsigned int *major, unsigned int *minor, unsigned int *micro);

typedef enum _Ender_Transfer_Type
{
	ENDER_TRANSFER_TYPE_FULL,
	ENDER_TRANSFER_TYPE_NONE,
	ENDER_TRANSFER_TYPE_CONTAINER,
	ENDER_TRANSFER_TYPE_CONTENT,
} Ender_Transfer_Type;

/**
 * @}
 */

#endif
