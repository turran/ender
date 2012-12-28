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
/**
 * @todo
 * - whenever an animation starts/ends send an event on the ender instance
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Escen_Animation
{
	char *name;
	Eina_Bool wait;
	Eina_List *keys;
};

struct _Escen_Animation_Key
{
	Escen_Animation *animation;
	Ender_Value *value;
	Escen_Time time;
	Etch_Interpolator_Type type;
};

static void _etch_data_from_ender_value(Etch_Data *d, Etch_Data_Type type, Ender_Value *v, Ender_Value_Type vtype)
{
	switch (vtype)
	{
		case ENDER_UINT32:
		d->data.u32 = ender_value_uint32_get(v);
		printf("uint32 setting etch value %d\n", d->data.u32);
		break;

		case ENDER_INT32:
		break;

		case ENDER_DOUBLE:
		d->data.d = ender_value_double_get(v);
		printf("setting etch value %g\n", d->data.d);
		break;

		case ENDER_ARGB:
		d->data.argb = ender_value_argb_get(v);
		printf("argb setting etch value %08x\n", d->data.argb);
		break;

		case ENDER_COLOR:
		d->data.argb = enesim_color_argb_to(ender_value_color_get(v));
		printf("color setting etch value %08x\n", d->data.argb);
		break;

		case ENDER_STRING:
		break;

		case ENDER_OBJECT:
		case ENDER_ENDER:
		case ENDER_LIST:
		case ENDER_STRUCT:
		printf(">>> complex property type\n");
		return;

		default:
		printf("Unsupported type %d\n", vtype);
		break;
	}

}
/*----------------------------------------------------------------------------*
 *                         The Etch animation callbacks                       *
 *----------------------------------------------------------------------------*/
static void _animation_callback(Etch_Animation_Keyframe *k,
		const Etch_Data *curr, const Etch_Data *prev, void *data)
{
	Escen_Animation_Instance *eai = data;
	Ender_Element *element;

	element = escen_instance_ender_get(eai->eei);
	if (ender_property_type_get(eai->property) == ENDER_COLOR)
	{
		uint32_t color;

		color = enesim_color_argb_from(curr->data.u32);
		ender_element_property_value_set(element, eai->property, color, NULL);
	}
	else
	{
		ender_element_property_value_set(element, eai->property, curr->data, NULL);
	}
}

static void _animation_start(Etch_Animation *a, void *data)
{
	printf("animation start\n");
}

static void _animation_end(Etch_Animation *a, void *data)
{
	Escen_Animation_Instance *eai = data;

	printf("animation end\n");
	/* whenever an animation ends, we should decrement the wait value */
	if (eai->ea->wait)
		escen_instance_wait_decrement(eai->eei);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* TODO rename this to escen_animation_ender_apply and remove the instance parameter */
void escen_animation_instance_set(Escen_Animation *ea, Escen_Instance *eei,
		Etch_Time time)
{
	Escen_Animation_Instance *eai;
	Escen_Animation_Key *eak;
	Ender_Property *property;
	Ender_Value_Type ptype;
	const Ender_Value_Type types[] = {
		[ETCH_UINT32] = ENDER_UINT32,
		[ETCH_INT32] = ENDER_INT32,
		[ETCH_FLOAT] = ENDER_DOUBLE,
		[ETCH_DOUBLE] = ENDER_DOUBLE,
		[ETCH_ARGB] = ENDER_ARGB,
		[ETCH_STRING] = ENDER_STRING,
	};
	Ender_Element *ender;
	Ender_Container *ec;
	Etch *etch;
	Etch_Data edata;
	Etch_Data_Type dtype;
	Etch_Animation *animation;
	Eina_List *l;

	ender = escen_instance_ender_get(eei);
	property = ender_element_property_get(ender, ea->name);
	ptype = ender_property_type_get(property);
	switch (ptype)
	{
		case ENDER_UINT32:
		dtype = ETCH_UINT32;
		break;

		case ENDER_INT32:
		dtype = ETCH_INT32;
		break;

		case ENDER_DOUBLE:
		dtype = ETCH_DOUBLE;
		break;

		case ENDER_ARGB:
		dtype = ETCH_ARGB;
		break;

		case ENDER_COLOR:
		dtype = ETCH_ARGB;
		break;

		case ENDER_STRING:
		dtype = ETCH_STRING;
		break;

		case ENDER_OBJECT:
		case ENDER_ENDER:
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_POINTER:
		case ENDER_VALUE:
		default:
		printf(">>> complex property type\n");
		return;
	}

	eai = calloc(1, sizeof(Escen_Animation_Instance));
	etch = escen_instance_etch_get(eei);
	animation = etch_animation_add(etch, dtype,
		_animation_callback, _animation_start, _animation_end, NULL, eai);
	etch_animation_offset_add(animation, time);

	eai->eei = eei;
	eai->ea = ea;
	eai->eta = animation;
	eai->property = property;
	escen_instance_animation_instance_add(eei, eai);
	ec = ender_property_container_get(property);
	/* set the type of the animation */
	EINA_LIST_FOREACH(ea->keys, l, eak)
	{
		Ender_Value_Type type;
		Ender_Value *v;
		Etch_Animation_Keyframe *k;

		type = types[dtype];
		if (!escen_copier_copy(type, ec, &v, eak->value, ender_value_type_get(eak->value)))
		{
			WRN("No way to copy the vaule");
			continue;
		}

		k = etch_animation_keyframe_add(animation);
		etch_animation_keyframe_time_set(k, eak->time);
		etch_animation_keyframe_type_set(k, eak->type);
		_etch_data_from_ender_value(&edata, dtype, v, type);
		etch_animation_keyframe_value_set(k, &edata);
		printf("animation keyframe added %d\n", eak->type);
		ender_value_unref(v);
	}

	/* let's enable it */
	etch_animation_enable(animation);
	/* FIXME what happens if we haven't added at least two keyframes? then we wont able to quit from the state */
	if (eai->ea->wait)
		escen_instance_wait_increment(eai->eei);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
Escen_Animation * escen_animation_new(const char *name, Eina_Bool wait)
{
	Escen_Animation *ea;

	ea = calloc(1, sizeof(Escen_Animation));
	ea->name = strdup(name);
	ea->wait = wait;

	return ea;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Animation_Key * escen_animation_key_add(Escen_Animation *ea, Ender_Value *value, Escen_Time time, Etch_Interpolator_Type atype)
{
	Escen_Animation_Key *eak;
	Ender_Value_Type ptype;

	if (!ea) return NULL;
	if (!value) return NULL;

#if 0
	Ender_Container *ec;
	Ender_Value *v;

	ptype = ender_property_type_get(ea->property);
	ec = ender_property_container_get(ea->property);

	if (!escen_copier_copy(ptype, ec, &v, value, ender_value_type_get(value)))
	{
		printf("animation key not added\n");
		return NULL;
	}
#endif
	eak = calloc(1, sizeof(Escen_Animation_Key));
	eak->animation = ea;
	eak->time = time;
	eak->type = atype;
	eak->value = value;
#if 0
	eak->value = v;
#endif
	ea->keys = eina_list_append(ea->keys, eak);

	return eak;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool escen_animation_state_waits(Escen_Animation *ea)
{
	if (!ea) return EINA_FALSE;

	return ea->wait;
}
