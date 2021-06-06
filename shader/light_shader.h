#pragma once

struct LightShaderArgs : public ShaderArgsBase {
	vec3  our_normal;
public:
	LightShaderArgs(const vec3 &normal);
	virtual SArgsPtr interp(const SArgsPtr &other, float t) const noexcept override;
	virtual SArgsPtr interp(const SArgsPtr &v1, const SArgsPtr &v2, const SArgsPtr &v3, 
							const vec3 &coords, float depth) const noexcept override;
	virtual void perspective_divide(float inverse_z) noexcept override;
};

struct LightShader : public ShaderBase {
public:
	virtual vec4 vertex(const Vertex &vertex, SArgsPtr &args) noexcept override;
	virtual bool fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept override;
};