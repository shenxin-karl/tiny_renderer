#pragma once

struct NormalMappingShaderArgs : public ShaderArgsBase {
	vec3  our_position;
	vec2  our_texcoord;
	vec3  our_eye_pos;
	vec3  our_light_dir;
public:
	NormalMappingShaderArgs(const vec3 &position, const vec2 &texcoord, const vec3 &eye_pos, const vec3 &light_dir);
	virtual SArgsPtr interp(const SArgsPtr &other, float t) const noexcept ;
	virtual SArgsPtr interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept override;
	virtual void perspective_divide(float inverse_z) noexcept override;
};

class NormalMappingShader : public ShaderBase {
	Texture2d	uniform_diffuse_texture;
	Texture2d	uniform_normal_texture;
	vec3		uniform_eye_pos;
	vec3		uniform_light_dir;
	vec3		uniform_light_ambient;
	vec3		uniform_light_diffuse;
	vec3		uniform_light_specular;
	float		uniform_specular_factor;
	mat3		uniform_normal_matrix;
public:
	virtual void initialize() noexcept override;
	virtual vec4 vertex(const Vertex &vertex, SArgsPtr &args) noexcept override;
	virtual bool fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept override;
};
