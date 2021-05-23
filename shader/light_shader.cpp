#include "common.h"

vec4 LightShader::vertex(const Vertex &vertex, int idx) {
	return mvp * vertex.position;
}

bool LightShader::fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) {
	vec3 normal = interp(vertices, &Vertex::normal);
	const vec3 &light_dir = get_uniform<vec3>("light_dir");
	float gray = std::max(dot(light_dir, normal), 0.f);
	color = { gray, gray, gray };
	return true;
}
