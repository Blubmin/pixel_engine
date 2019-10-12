#version 330

in vec2 v_uv;

// Uniforms
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D diffuse_texture;
};
uniform Material u_material;

out vec3 final_color;

void main() {
	vec3 zero = 0 * (u_material.ambient + u_material.specular) * u_material.shininess;

	final_color = u_material.diffuse + texture2D(u_material.diffuse_texture, v_uv).xyz + zero;
}