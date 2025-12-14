#version 110

uniform sampler2D tex;
uniform bool enable_texture;
uniform bool enable_texture_constant;
uniform bool enable_alpha;

uniform bool enable_fog;
uniform vec2 fog_delta;
uniform float fog_distance;
uniform vec3 fog_color;
uniform vec4 texture_const_color;

varying vec3 v_pos;
varying vec4 v_color;
varying vec2 v_texcoord;

void main() {
	vec4 tex_color = vec4(1.0);

	if(enable_texture) {
		if(enable_texture_constant) {
			tex_color = texture_const_color;
		} else {
			tex_color = texture2D(tex, v_texcoord);
		}
	}

	float v_fog = 0.0;

	if(enable_fog)
		v_fog = clamp((length(fog_delta + v_pos.xz) - (fog_distance - 9.0)) / 8.0, 0.0, 1.0);

	vec4 frag = v_color * tex_color;
	gl_FragColor = vec4(mix(frag.rgb, fog_color, v_fog), frag.a);

	if(enable_alpha && gl_FragColor.a < 0.0625)
		discard;
}
