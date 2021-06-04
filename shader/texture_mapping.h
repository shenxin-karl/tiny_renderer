#pragma once

class TextrueShader : public ShaderBase {
	const Texture2d *uniform_texture_ptr;
public:
	virtual void initialize() noexcept;
	vec4 vertex(const Vertex &vertex, int idx) noexcept;
	bool fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) noexcept;
};