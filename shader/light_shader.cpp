#include "common.h"

vec4 LightShader::vertex(const Vertex &vertex, int idx) noexcept {
	auto res = mvp * vertex.position;
	our_normal[idx] = vertex.normal / res.w();
	return res;
}

bool LightShader::fragment(const vec3 &point, vec3 &color) noexcept {
	vec3 normal = interp(our_normal);
	const vec3 &light_dir = get_uniform<vec3>("light_dir");
	float gray = std::max(dot(light_dir, normal), 0.f);
	color = { gray, gray, gray };
	return true;
}
