#pragma once

struct BlinnPhongShaderArgs : public ShaderArgsBase {
	vec3	our_normal;
	vec3	our_world_pos;
	vec2	our_texcoord;
public:
	BlinnPhongShaderArgs(const vec3 &normal, const vec3 &position, const vec2 texcoord);
	virtual SArgsPtr interp(const SArgsPtr & other, float t) const noexcept override;
	virtual SArgsPtr interp(const SArgsPtr & v2, const SArgsPtr & v3, const vec3 & coord, float depth) const noexcept override;
	virtual void perspective_divide(float inverse_z) noexcept override;
};

class BlinnPhong : public ShaderBase {
	vec3	    uniform_light_dir;
	vec3		uniform_eye_pos;
	Texture2d   uniform_diffuse_texture;
	float		uniform_specular_factor;
	vec3		uniform_light_ambient;
	vec3		uniform_light_diffuse;
	vec3		uniform_light_specular;
public:
	virtual void initialize() noexcept override;
	virtual vec4 vertex(const Vertex &vertex, SArgsPtr &args) noexcept override;
	virtual bool fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept override;
};
