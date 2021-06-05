#pragma once

class TextrueShader : public ShaderBase {
	const Texture2d *uniform_texture_ptr;
private:
	std::array<vec2, 3>		our_texcoords;
public:
	virtual void initialize() noexcept;
	vec4 vertex(const Vertex &vertex, int idx) noexcept;
	bool fragment(const vec3 &point, vec3 &color) noexcept;
};