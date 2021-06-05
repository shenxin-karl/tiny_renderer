#pragma once

struct LightShader : public ShaderBase {
private:
	std::array<vec3, 3> our_normal;
public:
	virtual vec4 vertex(const Vertex &vertex, int idx) noexcept override;
	virtual bool fragment(const vec3 &point, vec3 &color) noexcept override;
};