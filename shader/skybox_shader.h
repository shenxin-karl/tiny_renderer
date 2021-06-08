#pragma once

struct SkyboxShaderArgs : public ShaderArgsBase {
	vec3 our_texcoord;
public:
	SkyboxShaderArgs(const vec3 &texcoord);
	virtual SArgsPtr interp(const SArgsPtr &other, float t) const noexcept override;
	virtual SArgsPtr interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept override;
	virtual void perspective_divide(float inverse_z) noexcept override;
};

class SkyboxShader : public ShaderBase {
private:
	TextureCube uniform_skybox_texture;
	mat4		uniform_vp;
private:
	std::array<vec3, 3> our_world_position;
public:
	virtual void initialize() noexcept override;
	virtual vec4 vertex(const Vertex &vertex, SArgsPtr &args) noexcept override;
	virtual bool fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept override;
};
