#pragma once

struct ParallaxMappingShaderArgs : public ShaderArgsBase {
	vec2	our_texcoord;
	vec3	our_tangent_position;
	vec3	our_tangent_eye_pos;
	vec3	our_tangent_light_dir;
public:
	ParallaxMappingShaderArgs(const vec2 &texcoord, const vec3 &tangent_position, const vec3 &tangent_eye_pos,
							  const vec3 &tangent_light_dir);
	virtual SArgsPtr interp(const SArgsPtr &other, float t) const noexcept override;
	virtual SArgsPtr interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept override;
	virtual void perspective_divide(float inverse_z) noexcept override;
};


struct ParallaxMappingShader : public ShaderBase {
	mat3		uniform_normal_matrix;
	vec3		uniform_eye_pos;
	vec3		uniform_light_dir;
	Texture2d	uniform_diffuse_map;
	Texture2d	uniform_normal_map;
	Texture2d	uniform_depth_map;
	float		uniform_height_scale;
	vec3		uniform_light_ambient;
	vec3		uniform_light_diffuse;
	vec3		uniform_light_specular;
	float		uniform_specular_factor;
public:
	virtual void initialize() noexcept override;
	virtual vec4 vertex(const Vertex &vertex, SArgsPtr &args) noexcept override;
	virtual bool fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept override;
	vec2 parallax_mapping_texcoord(const vec3 &view_dir, const vec2 &texcoord) const;
	vec2 steep_parallax_mapping_texcoord(const vec3 &view_dir, vec2 texcoord) const;
	vec2 parallax_occlusion_mapping(const vec3 &view_dir, vec2 texcoord) const;
};