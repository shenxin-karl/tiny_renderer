#pragma once

class DepthShader : public ShaderBase {
	float near;
	float denominator;
public:
	virtual void initialize() noexcept override;
	virtual vec4 vertex(const Vertex &vertex, int idx) noexcept override;
	virtual bool fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) noexcept override;
};