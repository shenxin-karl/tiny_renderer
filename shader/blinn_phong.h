#pragma once

class BlinnPhong : public ShaderBase {
	std::array<vec3, 3>	our_normal;
	std::array<vec3, 3> our_world_pos;
private:
	const vec3 *light_dir_ptr = nullptr;
	const Texture2d *diffuse_texture_ptr = nullptr;
	const vec3 *light_ambient_ptr = nullptr;
	const vec3 *light_diffuse_ptr = nullptr;
	const vec3 *light_specular_ptr = nullptr;
	const vec3 *eye_pos_ptr = nullptr;
	const float *factor_ptr = nullptr;
public:
	virtual void initialize() noexcept override;
	virtual vec4 vertex(const Vertex &vertex, int idx) noexcept override;
	virtual bool fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) noexcept override;
};