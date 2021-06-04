#pragma once

class SkyboxShader : public ShaderBase {
	const TextureCube *uniform_skybox_texture;
	mat4 uniform_vp;
private:
	std::array<vec3, 3> our_world_position;
public:
	virtual void initialize() noexcept override;
	virtual vec4 vertex(const Vertex &vertex, int idx) noexcept override;
	virtual bool fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) noexcept override;
};