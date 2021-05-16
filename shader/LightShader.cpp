#include "common.h"

vec4 LightShader::vertex(const Vertex &vertex, int idx) {
	return (viewport * projection * view * model) * vertex.position;
}

bool LightShader::fragment(std::array<Vertex *, 3> &vertices, vec3 &color) {
	vec3 normal = interp(vertices[0]->normal, vertices[1]->normal, vertices[2]->normal);
	normal.normalize();
	
	const vec3 &light_dir = get_uniform<vec3>("light_dir");
	float dot_value = dot(normal, light_dir);
	color = { dot_value, dot_value, dot_value };
	return true;
}
