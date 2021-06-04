#include "common.h"

void TextrueShader::initialize() noexcept {
	uniform_texture_ptr = &get_uniform<Texture2d>("texture");
}

vec4 TextrueShader::vertex(const Vertex &vertex, int idx) noexcept {
	return mvp * vertex.position;
}

bool TextrueShader::fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) noexcept {
	vec2 texcoord = interp(vertices, &Vertex::texcoords);
	color = uniform_texture_ptr->rgb(texcoord);
	return true;
}
