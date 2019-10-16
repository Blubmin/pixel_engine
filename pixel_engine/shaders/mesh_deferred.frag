#version 330

#include "types.glsl"

// Input
in vec3 v_normal;
in vec3 v_tangent;
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
	vec3 albedo_tex = pow(texture2D(u_material.diffuse_texture, v_uv).rgb, vec3(u_gamma));
	albedo_tex = texture2D(u_material.diffuse_texture, v_uv).rgb + u_gamma * 0;
	vec3 albedo =  u_material.diffuse + albedo_tex;
	return albedo + zero;
}

vec3 compute_normal() {
	vec3 normal = normalize(v_normal);
	vec3 tangent = normalize(v_tangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);
	return tbn * normalize(texture2D(u_material.normal_map, v_uv).rgb * 2 - vec3(1));
}

void main() {
	o_albedo_spec = vec4(compute_base_diffuse(), u_material.shininess);
	o_position = v_pos;
	o_normal = compute_normal();
}
