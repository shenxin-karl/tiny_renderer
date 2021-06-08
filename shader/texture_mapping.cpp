#include "common.h"

TextrueShaderArgs::TextrueShaderArgs(const vec2 &texcoord) : our_texcoord(texcoord) {
}

SArgsPtr TextrueShaderArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const TextrueShaderArgs *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &TextrueShaderArgs::our_texcoord, t)
	);
}

SArgsPtr TextrueShaderArgs::interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept {
	const TextrueShaderArgs *v1_ptr = this;
	const TextrueShaderArgs *v2_ptr = SArgsPtr_cast(v2);
	const TextrueShaderArgs *v3_ptr = SArgsPtr_cast(v3);
	return Make_ArgsPtr(
		common_interp(v1_ptr, v2_ptr, v3_ptr, &TextrueShaderArgs::our_texcoord, coord, depth)
	);
}

void TextrueShaderArgs::perspective_divide(float inverse_z) noexcept {
	our_texcoord *= inverse_z;
}

void TextrueShader::initialize() noexcept {
	uniform_texture = get_uniform<Texture2d>("texture");
}

vec4 TextrueShader::vertex(const Vertex &vertex, SArgsPtr &args) noexcept {
	args = std::make_shared<TextrueShaderArgs>(TextrueShaderArgs{
		vertex.texcoords
	});
	return mvp * vertex.position;
}

bool TextrueShader::fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept {
	const TextrueShaderArgs *args_ptr = static_cast<const TextrueShaderArgs *>(args.get());
	color = uniform_texture.rgb(args_ptr->our_texcoord);
	return true;
}
