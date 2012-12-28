/* ESCEN - Ender's based scene loader
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
#include "Escen.h"
#include "escen_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Hash *_color_lut = NULL;

static void _color_lut_generate(Eina_Hash **lut)
{
	typedef struct _Escen_Color_Entry
	{
		const char *name;
		Enesim_Color color;
	} Escen_Color_Entry;

	Escen_Color_Entry colors[] = {
		{ "AliceBlue", 0xFFF0F8FF },
		{ "AntiqueWhite", 0xFFFAEBD7 },
		{ "Aqua", 0xFF00FFFF },
		{ "Aquamarine", 0xFF7FFFD4 },
		{ "Azure", 0xFFF0FFFF },
		{ "Beige", 0xFFF5F5DC },
		{ "Bisque", 0xFFFFE4C4 },
		{ "Black", 0xFF000000 },
		{ "BlanchedAlmond", 0xFFFFEBCD },
		{ "Blue", 0xFF0000FF },
		{ "BlueViolet", 0xFF8A2BE2 },
		{ "Brown", 0xFFA52A2A },
		{ "BurlyWood", 0xFFDEB887 },
		{ "CadetBlue", 0xFF5F9EA0 },
		{ "Chartreuse", 0xFF7FFF00 },
		{ "Chocolate", 0xFFD2691E },
		{ "Coral", 0xFFFF7F50 },
		{ "CornflowerBlue", 0xFF6495ED },
		{ "Cornsilk", 0xFFFFF8DC },
		{ "Crimson", 0xFFDC143C },
		{ "Cyan", 0xFF00FFFF },
		{ "DarkBlue", 0xFF00008B },
		{ "DarkCyan", 0xFF008B8B },
		{ "DarkGoldenRod", 0xFFB8860B },
		{ "DarkGray", 0xFFA9A9A9 },
		{ "DarkGrey", 0xFFA9A9A9 },
		{ "DarkGreen", 0xFF006400 },
		{ "DarkKhaki", 0xFFBDB76B },
		{ "DarkMagenta", 0xFF8B008B },
		{ "DarkOliveGreen", 0xFF556B2F },
		{ "Darkorange", 0xFFFF8C00 },
		{ "DarkOrchid", 0xFF9932CC },
		{ "DarkRed", 0xFF8B0000 },
		{ "DarkSalmon", 0xFFE9967A },
		{ "DarkSeaGreen", 0xFF8FBC8F },
		{ "DarkSlateBlue", 0xFF483D8B },
		{ "DarkSlateGray", 0xFF2F4F4F },
		{ "DarkSlateGrey", 0xFF2F4F4F },
		{ "DarkTurquoise", 0xFF00CED1 },
		{ "DarkViolet", 0xFF9400D3 },
		{ "DeepPink", 0xFFFF1493 },
		{ "DeepSkyBlue", 0xFF00BFFF },
		{ "DimGray", 0xFF696969 },
		{ "DimGrey", 0xFF696969 },
		{ "DodgerBlue", 0xFF1E90FF },
		{ "FireBrick", 0xFFB22222 },
		{ "FloralWhite", 0xFFFFFAF0 },
		{ "ForestGreen", 0xFF228B22 },
		{ "Fuchsia", 0xFFFF00FF },
		{ "Gainsboro", 0xFFDCDCDC },
		{ "GhostWhite", 0xFFF8F8FF },
		{ "Gold", 0xFFFFD700 },
		{ "GoldenRod", 0xFFDAA520 },
		{ "Gray", 0xFF808080 },
		{ "Grey", 0xFF808080 },
		{ "Green", 0xFF008000 },
		{ "GreenYellow", 0xFFADFF2F },
		{ "HoneyDew", 0xFFF0FFF0 },
		{ "HotPink", 0xFFFF69B4 },
		{ "IndianRed", 0xFFCD5C5C },
		{ "Indigo", 0xFF4B0082 },
		{ "Ivory", 0xFFFFFFF0 },
		{ "Khaki", 0xFFF0E68C },
		{ "Lavender", 0xFFE6E6FA },
		{ "LavenderBlush", 0xFFFFF0F5 },
		{ "LawnGreen", 0xFF7CFC00 },
		{ "LemonChiffon", 0xFFFFFACD },
		{ "LightBlue", 0xFFADD8E6 },
		{ "LightCoral", 0xFFF08080 },
		{ "LightCyan", 0xFFE0FFFF },
		{ "LightGoldenRodYellow", 0xFFFAFAD2 },
		{ "LightGray", 0xFFD3D3D3 },
		{ "LightGrey", 0xFFD3D3D3 },
		{ "LightGreen", 0xFF90EE90 },
		{ "LightPink", 0xFFFFB6C1 },
		{ "LightSalmon", 0xFFFFA07A },
		{ "LightSeaGreen", 0xFF20B2AA },
		{ "LightSkyBlue", 0xFF87CEFA },
		{ "LightSlateGray", 0xFF778899 },
		{ "LightSlateGrey", 0xFF778899 },
		{ "LightSteelBlue", 0xFFB0C4DE },
		{ "LightYellow", 0xFFFFFFE0 },
		{ "Lime", 0xFF00FF00 },
		{ "LimeGreen", 0xFF32CD32 },
		{ "Linen", 0xFFFAF0E6 },
		{ "Magenta", 0xFFFF00FF },
		{ "Maroon", 0xFF800000 },
		{ "MediumAquaMarine", 0xFF66CDAA },
		{ "MediumBlue", 0xFF0000CD },
		{ "MediumOrchid", 0xFFBA55D3 },
		{ "MediumPurple", 0xFF9370D8 },
		{ "MediumSeaGreen", 0xFF3CB371 },
		{ "MediumSlateBlue", 0xFF7B68EE },
		{ "MediumSpringGreen", 0xFF00FA9A },
		{ "MediumTurquoise", 0xFF48D1CC },
		{ "MediumVioletRed", 0xFFC71585 },
		{ "MidnightBlue", 0xFF191970 },
		{ "MintCream", 0xFFF5FFFA },
		{ "MistyRose", 0xFFFFE4E1 },
		{ "Moccasin", 0xFFFFE4B5 },
		{ "NavajoWhite", 0xFFFFDEAD },
		{ "Navy", 0xFF000080 },
		{ "OldLace", 0xFFFDF5E6 },
		{ "Olive", 0xFF808000 },
		{ "OliveDrab", 0xFF6B8E23 },
		{ "Orange", 0xFFFFA500 },
		{ "OrangeRed", 0xFFFF4500 },
		{ "Orchid", 0xFFDA70D6 },
		{ "PaleGoldenRod", 0xFFEEE8AA },
		{ "PaleGreen", 0xFF98FB98 },
		{ "PaleTurquoise", 0xFFAFEEEE },
		{ "PaleVioletRed", 0xFFD87093 },
		{ "PapayaWhip", 0xFFFFEFD5 },
		{ "PeachPuff", 0xFFFFDAB9 },
		{ "Peru", 0xFFCD853F },
		{ "Pink", 0xFFFFC0CB },
		{ "Plum", 0xFFDDA0DD },
		{ "PowderBlue", 0xFFB0E0E6 },
		{ "Purple", 0xFF800080 },
		{ "Red", 0xFFFF0000 },
		{ "RosyBrown", 0xFFBC8F8F },
		{ "RoyalBlue", 0xFF4169E1 },
		{ "SaddleBrown", 0xFF8B4513 },
		{ "Salmon", 0xFFFA8072 },
		{ "SandyBrown", 0xFFF4A460 },
		{ "SeaGreen", 0xFF2E8B57 },
		{ "SeaShell", 0xFFFFF5EE },
		{ "Sienna", 0xFFA0522D },
		{ "Silver", 0xFFC0C0C0 },
		{ "SkyBlue", 0xFF87CEEB },
		{ "SlateBlue", 0xFF6A5ACD },
		{ "SlateGray", 0xFF708090 },
		{ "SlateGrey", 0xFF708090 },
		{ "Snow", 0xFFFFFAFA },
		{ "SpringGreen", 0xFF00FF7F },
		{ "SteelBlue", 0xFF4682B4 },
		{ "Tan", 0xFFD2B48C },
		{ "Teal", 0xFF008080 },
		{ "Thistle", 0xFFD8BFD8 },
		{ "Tomato", 0xFFFF6347 },
		{ "Turquoise", 0xFF40E0D0 },
		{ "Violet", 0xFFEE82EE },
		{ "Wheat", 0xFFF5DEB3 },
		{ "White", 0xFFFFFFFF },
		{ "WhiteSmoke", 0xFFF5F5F5 },
		{ "Yellow", 0xFFFFFF00 },
		{ "YellowGreen", 0xFF9ACD32 }
	};
	int i;

	*lut = eina_hash_string_superfast_new(NULL);
	for (i = 0; i < sizeof(colors) / sizeof(Escen_Color_Entry); i++)
	{
		Enesim_Color *color;

		color = malloc(sizeof(Enesim_Color));
		*color = colors[i].color;
		eina_hash_add(*lut, colors[i].name, color);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Enesim_Argb enesim_color_lut_get(const char *name)
{
	Enesim_Argb *argb;

	argb = eina_hash_find(_color_lut, name);
	if (!argb) return 0xff000000;
	return *argb;
}

void escen_misc_init(void)
{
	_color_lut_generate(&_color_lut);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
