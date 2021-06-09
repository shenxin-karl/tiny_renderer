#pragma once

struct SkyboxReflectArgs : public ShaderArgsBase {
	vec3	our_position;
	vec3	our_normal;
public:
	SkyboxReflectArgs(const vec3 &position, const vec3 &normal);
	virtual SArgsPtr interp(const SArgsPtr &other, float t) const noexcept;
	virtual SArgsPtr interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept;
	virtual void perspective_divide(float inverse_z) noexcept;
};

struct SkyboxReflect : public ShaderBase {
	TextureCube	 uniform_skybox_texture;
	mat3		 uniform_normal_matrix;
	vec3		 uniform_eye_pos;
public:
	virtual void initialize() noexcept override;		
	virtual vec4 vertex(const Vertex &vertex, SArgsPtr &args) noexcept override;
	virtual bool fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept override;
};