/*
	Copyright (c) 2023 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	CavEX is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with CavEX.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>

#include "render_model.h"

void render_model_box(mat4 view, vec3 position, vec3 pivot, vec3 rotation,
					  ivec2 origin, ivec3 box, float padding, bool mirror,
					  float brightness) {
	assert(view && position && pivot && rotation && origin && box
		   && padding >= -GLM_FLT_EPSILON && brightness >= -GLM_FLT_EPSILON
		   && brightness <= 1.0F + GLM_FLT_EPSILON);

	int sw = 256 / 64;
	int sh = 256 / 32;

	mat4 model;
	glm_translate_make(model, position);
	glm_rotate_z(model, glm_rad(rotation[2]), model);
	glm_rotate_y(model, glm_rad(rotation[1]), model);
	glm_rotate_x(model, glm_rad(rotation[0]), model);
	glm_translate(
		model,
		(vec3) {-pivot[0] - padding, -pivot[1] - padding, -pivot[2] - padding});
	glm_scale(model,
			  (vec3) {padding * 2.0F / box[0] + 1.0F,
					  padding * 2.0F / box[2] + 1.0F,
					  padding * 2.0F / box[1] + 1.0F});

	mat4 modelview;
	glm_mat4_mul(view, model, modelview);
	gfx_matrix_modelview(modelview);

	uint8_t light = roundf(brightness * 0xFF);

	// top, bottom, back, front, right, left
	gfx_draw_quads(
		24,
		(int16_t[]) {
			0,		box[2], 0,		box[0], box[2], 0,
			box[0], box[2], box[1], 0,		box[2], box[1],

			0,		0,		0,		0,		0,		box[1],
			box[0], 0,		box[1], box[0], 0,		0,

			0,		box[2], 0,		0,		0,		0,
			box[0], 0,		0,		box[0], box[2], 0,

			0,		box[2], box[1], box[0], box[2], box[1],
			box[0], 0,		box[1], 0,		0,		box[1],

			box[0], 0,		0,		box[0], 0,		box[1],
			box[0], box[2], box[1], box[0], box[2], 0,

			0,		0,		0,		0,		box[2], 0,
			0,		box[2], box[1], 0,		0,		box[1],
		},
		(uint8_t[]) {
			light, light, light, 0xFF, light, light, light, 0xFF,
			light, light, light, 0xFF, light, light, light, 0xFF,

			light, light, light, 0xFF, light, light, light, 0xFF,
			light, light, light, 0xFF, light, light, light, 0xFF,

			light, light, light, 0xFF, light, light, light, 0xFF,
			light, light, light, 0xFF, light, light, light, 0xFF,

			light, light, light, 0xFF, light, light, light, 0xFF,
			light, light, light, 0xFF, light, light, light, 0xFF,

			light, light, light, 0xFF, light, light, light, 0xFF,
			light, light, light, 0xFF, light, light, light, 0xFF,

			light, light, light, 0xFF, light, light, light, 0xFF,
			light, light, light, 0xFF, light, light, light, 0xFF,
		},
		(uint16_t[]) {
			(mirror ? (origin[0] + box[0]) : origin[0]) * sw,
			(origin[1] - box[1]) * sh,
			(mirror ? origin[0] : (origin[0] + box[0])) * sw,
			(origin[1] - box[1]) * sh,
			(mirror ? origin[0] : (origin[0] + box[0])) * sw,
			origin[1] * sh,
			(mirror ? (origin[0] + box[0]) : origin[0]) * sw,
			origin[1] * sh,

			(mirror ? (origin[0] + 2 * box[0]) : (origin[0] + box[0])) * sw,
			origin[1] * sh,
			(mirror ? (origin[0] + 2 * box[0]) : (origin[0] + box[0])) * sw,
			(origin[1] - box[1]) * sh,
			(mirror ? (origin[0] + box[0]) : (origin[0] + 2 * box[0])) * sw,
			(origin[1] - box[1]) * sh,
			(mirror ? (origin[0] + box[0]) : (origin[0] + 2 * box[0])) * sw,
			origin[1] * sh,

			(mirror ? (origin[0] + box[0] + box[1]) :
					  (origin[0] + 2 * box[0] + box[1]))
				* sw,
			origin[1] * sh,
			(mirror ? (origin[0] + box[0] + box[1]) :
					  (origin[0] + 2 * box[0] + box[1]))
				* sw,
			(origin[1] + box[2]) * sh,
			(mirror ? (origin[0] + 2 * box[0] + box[1]) :
					  (origin[0] + box[0] + box[1]))
				* sw,
			(origin[1] + box[2]) * sh,
			(mirror ? (origin[0] + 2 * box[0] + box[1]) :
					  (origin[0] + box[0] + box[1]))
				* sw,
			origin[1] * sh,

			(mirror ? (origin[0] + box[0]) : origin[0]) * sw,
			origin[1] * sh,
			(mirror ? origin[0] : (origin[0] + box[0])) * sw,
			origin[1] * sh,
			(mirror ? origin[0] : (origin[0] + box[0])) * sw,
			(origin[1] + box[2]) * sh,
			(mirror ? (origin[0] + box[0]) : origin[0]) * sw,
			(origin[1] + box[2]) * sh,

			(origin[0] + box[0] + box[1]) * sw,
			(origin[1] + box[2]) * sh,
			(origin[0] + box[0]) * sw,
			(origin[1] + box[2]) * sh,
			(origin[0] + box[0]) * sw,
			origin[1] * sh,
			(origin[0] + box[0] + box[1]) * sw,
			origin[1] * sh,

			(origin[0] - box[1]) * sw,
			(origin[1] + box[2]) * sh,
			(origin[0] - box[1]) * sw,
			origin[1] * sh,
			origin[0] * sw,
			origin[1] * sh,
			origin[0] * sw,
			(origin[1] + box[2]) * sh,
		});
}

void render_model_player(mat4 mv, float head_pitch, float head_yaw,
						 float foot_angle, float arm_angle,
						 struct item_data* held_item, struct item_data* helmet,
						 struct item_data* chestplate,
						 struct item_data* leggings, struct item_data* boots) {
	assert(mv);

	struct item* held_item_it = held_item ? item_get(held_item) : NULL;
	struct item* helmet_it = helmet ? item_get(helmet) : NULL;
	struct item* chestplate_it = chestplate ? item_get(chestplate) : NULL;
	struct item* leggings_it = leggings ? item_get(leggings) : NULL;
	struct item* boots_it = boots ? item_get(boots) : NULL;

	gfx_bind_texture(&texture_mob_char);

	render_model_box(mv, (vec3) {0.0F, -4.0F, 0.0F}, (vec3) {4.0F, 0.0F, 4.0F},
					 (vec3) {head_pitch, head_yaw, 0.0F}, (ivec2) {8, 8},
					 (ivec3) {8, 8, 8}, 0.0F, false, 1.0F);
	render_model_box(mv, (vec3) {0.0F, -4.0F, 0.0F}, (vec3) {4.0F, 0.0F, 4.0F},
					 (vec3) {head_pitch, head_yaw, 0.0F}, (ivec2) {40, 8},
					 (ivec3) {8, 8, 8}, 0.5F, false, 1.0F);

	render_model_box(mv, (vec3) {0.0F, -4.0F, 0.0F}, (vec3) {4.0F, 12.0F, 2.0F},
					 (vec3) {0.0F, 0.0F, 0.0F}, (ivec2) {20, 20},
					 (ivec3) {8, 4, 12}, 0.0F, false, 1.0F);
	render_model_box(mv, (vec3) {2.0F, -16.0F, 0.0F},
					 (vec3) {2.0F, 12.0F, 2.0F},
					 (vec3) {foot_angle, 0.0F, 0.0F}, (ivec2) {4, 20},
					 (ivec3) {4, 4, 12}, 0.0F, true, 1.0F);
	render_model_box(mv, (vec3) {-2.0F, -16.0F, 0.0F},
					 (vec3) {2.0F, 12.0F, 2.0F},
					 (vec3) {-foot_angle, 0.0F, 0.0F}, (ivec2) {4, 20},
					 (ivec3) {4, 4, 12}, 0.0F, false, 1.0F);
	render_model_box(mv, (vec3) {6.0F, -4.0F, 0.0F}, (vec3) {2.0F, 12.0F, 2.0F},
					 (vec3) {arm_angle, 0.0F, 4.0F}, (ivec2) {44, 20},
					 (ivec3) {4, 4, 12}, 0.0F, true, 1.0F);
	render_model_box(
		mv, (vec3) {-6.0F, -4.0F, 0.0F}, (vec3) {2.0F, 12.0F, 2.0F},
		(vec3) {(held_item_it ? -22.5F : 0.0F) - arm_angle, 0.0F, -4.0F},
		(ivec2) {44, 20}, (ivec3) {4, 4, 12}, 0.0F, false, 1.0F);

	if(held_item_it) {
		mat4 model;
		glm_translate_make(model, (vec3) {-6.0F, -4.0F, 0.0F});
		glm_rotate_z(model, glm_rad(-4.0F), model);
		glm_rotate_x(model, glm_rad(-22.5F - arm_angle), model);

		mat4 mv_item;
		glm_mat4_mul(mv, model, mv_item);
		held_item_it->renderItem(held_item_it, held_item, mv_item, true,
								 R_ITEM_ENV_THIRDPERSON);
	}

	struct tex_gfx* armor_texture[][2] = {
		[ARMOR_TIER_BLOCK] = {NULL, NULL},
		[ARMOR_TIER_CHAIN] = {&texture_armor_chain1, &texture_armor_chain2},
		[ARMOR_TIER_CLOTH] = {&texture_armor_cloth1, &texture_armor_cloth2},
		[ARMOR_TIER_GOLD] = {&texture_armor_gold1, &texture_armor_gold2},
		[ARMOR_TIER_IRON] = {&texture_armor_iron1, &texture_armor_iron2},
		[ARMOR_TIER_DIAMOND]
		= {&texture_armor_diamond1, &texture_armor_diamond2},
	};

	gfx_cull_func(MODE_NONE);

	if(helmet_it && helmet_it->armor.is_armor
	   && helmet_it->armor.type == ARMOR_TYPE_HELMET) {
		if(helmet_it->armor.tier == ARMOR_TIER_BLOCK) {
			mat4 model;
			glm_translate_make(model, (vec3) {0.0F, -4.0F, 0.0F});
			glm_rotate_y(model, glm_rad(head_yaw), model);
			glm_rotate_x(model, glm_rad(head_pitch), model);
			glm_translate(model, (vec3) {4.5F, -0.5F, 4.5F});
			glm_rotate_y(model, glm_rad(180.0F), model);
			glm_scale_uni(model, 9.0F);

			mat4 mv_item;
			glm_mat4_mul(mv, model, mv_item);

			helmet_it->renderItem(helmet_it, helmet, mv_item, true,
								  R_ITEM_ENV_ENTITY);
		} else {
			gfx_bind_texture(armor_texture[helmet_it->armor.tier][0]);
			render_model_box(
				mv, (vec3) {0.0F, -4.0F, 0.0F}, (vec3) {4.0F, 0.0F, 4.0F},
				(vec3) {head_pitch, head_yaw, 0.0F}, (ivec2) {8, 8},
				(ivec3) {8, 8, 8}, 1.0F, false, 1.0F);
		}
	}

	if(chestplate_it && chestplate_it->armor.is_armor
	   && chestplate_it->armor.type == ARMOR_TYPE_CHESTPLATE) {
		gfx_bind_texture(armor_texture[chestplate_it->armor.tier][0]);
		render_model_box(mv, (vec3) {0.0F, -4.0F, 0.0F},
						 (vec3) {4.0F, 12.0F, 2.0F}, (vec3) {0.0F, 0.0F, 0.0F},
						 (ivec2) {20, 20}, (ivec3) {8, 4, 12}, 1.0F, false,
						 1.0F);
		render_model_box(mv, (vec3) {6.0F, -4.0F, 0.0F},
						 (vec3) {2.0F, 12.0F, 2.0F},
						 (vec3) {arm_angle, 0.0F, 4.0F}, (ivec2) {44, 20},
						 (ivec3) {4, 4, 12}, 1.0F, true, 1.0F);
		render_model_box(
			mv, (vec3) {-6.0F, -4.0F, 0.0F}, (vec3) {2.0F, 12.0F, 2.0F},
			(vec3) {(held_item_it ? -22.5F : 0.0F) - arm_angle, 0.0F, -4.0F},
			(ivec2) {44, 20}, (ivec3) {4, 4, 12}, 1.0F, false, 1.0F);
	}

	if(leggings_it && leggings_it->armor.is_armor
	   && leggings_it->armor.type == ARMOR_TYPE_LEGGINGS) {
		gfx_bind_texture(armor_texture[leggings_it->armor.tier][1]);
		render_model_box(mv, (vec3) {0.0F, -4.0F, 0.0F},
						 (vec3) {4.0F, 12.0F, 2.0F}, (vec3) {0.0F, 0.0F, 0.0F},
						 (ivec2) {20, 20}, (ivec3) {8, 4, 12}, 0.5F, false,
						 1.0F);
		render_model_box(mv, (vec3) {2.0F, -16.0F, 0.0F},
						 (vec3) {2.0F, 12.0F, 2.0F},
						 (vec3) {foot_angle, 0.0F, 0.0F}, (ivec2) {4, 20},
						 (ivec3) {4, 4, 12}, 0.5F, true, 1.0F);
		render_model_box(mv, (vec3) {-2.0F, -16.0F, 0.0F},
						 (vec3) {2.0F, 12.0F, 2.0F},
						 (vec3) {-foot_angle, 0.0F, 0.0F}, (ivec2) {4, 20},
						 (ivec3) {4, 4, 12}, 0.5F, false, 1.0F);
	}

	if(boots_it && boots_it->armor.is_armor
	   && boots_it->armor.type == ARMOR_TYPE_BOOTS) {
		gfx_bind_texture(armor_texture[boots_it->armor.tier][0]);
		render_model_box(mv, (vec3) {2.0F, -16.0F, 0.0F},
						 (vec3) {2.0F, 12.0F, 2.0F},
						 (vec3) {foot_angle, 0.0F, 0.0F}, (ivec2) {4, 20},
						 (ivec3) {4, 4, 12}, 1.0F, true, 1.0F);
		render_model_box(mv, (vec3) {-2.0F, -16.0F, 0.0F},
						 (vec3) {2.0F, 12.0F, 2.0F},
						 (vec3) {-foot_angle, 0.0F, 0.0F}, (ivec2) {4, 20},
						 (ivec3) {4, 4, 12}, 1.0F, false, 1.0F);
	}

	gfx_cull_func(MODE_BACK);
}