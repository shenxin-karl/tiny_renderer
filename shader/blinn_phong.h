#pragma once

#if 0
struct BlinnPhongShaderArgs : public ShaderArgsBase {
	vec3	our_normal;
	vec3	our_world_pos;
	vec3	our_texcoord;
public:
	
	virtual SArgsPtr interp(const SArgsPtr & other, float t) const noexcept override;
	virtual SArgsPtr interp(const SArgsPtr & v1, const SArgsPtr & v2, const SArgsPtr & v3,
		const vec3 & coord, float depth) const noexcept override;
	virtual void perspective_divide(float inverse_z) noexcept override;
};

class BlinnPhong : public ShaderBase {
	std::array<vec3, 3>	our_normal;
	std::array<vec3, 3> our_world_pos;
	std::array<vec2, 3> our_texcoords;
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
	virtual bool fragment(const vec3 &point, vec3 &color) noexcept override;
};
#endif