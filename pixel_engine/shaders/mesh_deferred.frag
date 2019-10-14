#version 330

#include "types.glsl"

// Input
in vec3 v_normal;
in vec3 v_pos;
in vec2 v_uv;

// Uniforms
uniform Material u_material;
uniform float u_gamma;

// Output
layout(location = 0) out vec4 o_albedo_spec;
layout(location = 1) out vec3 o_position;
layout(location = 2) out vec3 o_normal;

vec3 compute_base_diffuse() { 
	vec3 zero = 0 * (u_material.ambient + u_material.specular) * 
		u_material.shininess;
	vec3 albedo_tex = pow(texture2D(u_material.diffuse_texture, v_uv).xyz, vec3(u_gamma));
	vec3 albedo =  u_material.diffuse + albedo_tex;
	return albedo + zero;
}

void main() {
	o_albedo_spec = vec4(compute_base_diffuse(), u_material.shininess);
	o_position = v_pos;
	o_normal = v_normal;
}
