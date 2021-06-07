#include "common.h"

#if 0
void TextrueShader::initialize() noexcept {
	uniform_texture_ptr = &get_uniform<Texture2d>("texture");
}

vec4 TextrueShader::vertex(const Vertex &vertex, int idx) noexcept {
	auto res = mvp * vertex.position;
	our_texcoords[idx] = vertex.texcoords / res.w();
	return res;
}

bool TextrueShader::fragment(const vec3 &point, vec3 &color) noexcept {
	vec2 texcoord = interp(our_texcoords);
	color = uniform_texture_ptr->rgb(texcoord);
	return true;
}
#endif