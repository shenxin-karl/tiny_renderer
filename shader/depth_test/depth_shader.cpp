#include "common.h"
#if 0
void DepthShader::initialize() noexcept {
	near =  get_uniform<float>("near");
	float far =  get_uniform<float>("far");
	denominator = (far - near);
}

vec4 DepthShader::vertex(const Vertex &vertex, int idx) noexcept {
	return mvp * vertex.position;
}

bool DepthShader::fragment(const vec3 &point, vec3 &color) noexcept {
	float depth = point.z();
	float gray = (depth - this->near) / this->denominator;
	color = { gray, gray, gray };
	return true;
}
#endif