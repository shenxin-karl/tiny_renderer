#pragma once

struct LightShader : public ShaderBase {
	virtual vec4 vertex(const Vertex &vertex, int idx) override;
	virtual bool fragment(std::array<Vertex *, 3> &vertices, vec3 &color) override;
};