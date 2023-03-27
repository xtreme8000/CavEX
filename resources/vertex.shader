#version 110

uniform mat4 mv;
uniform mat4 proj;
uniform mat4 texm;

uniform bool enable_lighting;
uniform float lighting[256];

attribute vec3 a_pos;
attribute vec4 a_color;
attribute vec2 a_texcoord;
attribute vec2 a_light;

varying vec3 v_pos;
varying vec4 v_color;
varying vec2 v_texcoord;

void main() {
	if(enable_lighting) {
		v_color = vec4(vec3(lighting[int(a_light.x) + int(a_light.y) * 16]), 1.0);
	} else {
		v_color = a_color;
	}

	v_pos = a_pos;
	v_texcoord = (texm * vec4(a_texcoord, 0.0, 1.0)).xy;
	gl_Position = proj * mv  * vec4(a_pos, 1.0);
}
