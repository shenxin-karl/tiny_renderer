#pragma once

class BlinnPhong : public ShaderBase {
	std::array<vec3, 3>	our_normal;
	std::array<vec3, 3> our_world_pos;
public:
	virtual void initialize() noexcept override;
	virtual vec4 vertex(const Vertex &vertex, int idx) noexcept override;
	virtual bool fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) noexcept override;
};