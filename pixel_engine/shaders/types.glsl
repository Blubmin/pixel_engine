struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D diffuse_texture;
};

struct PointLight {
    vec3 pos;
    vec3 color;
    float linear_attenuation;
    float quadratic_attenuation;
};

struct DirectionalLight {
	vec3 direction;
	float strength;
	vec3 color;
};