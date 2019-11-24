#version 330

#include "types.glsl"

in vec2 v_uv;

// Uniforms
uniform Material u_material;
uniform float u_gamma;

out vec3 final_color;

void main() {
	vec3 zero = 0 * (u_material.ambient + u_material.specular) * 
		u_material.shininess * texture2D(u_material.normal_map, vec2(0, 0)).xyz;

	final_color = u_material.diffuse + zero +
		pow(texture2D(u_material.diffuse_texture, v_uv).xyz, vec3(u_gamma));
	final_color = pow(final_color, vec3(1 / u_gamma));
}