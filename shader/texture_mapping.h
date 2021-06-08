#pragma once

struct TextrueShaderArgs : public ShaderArgsBase {
	vec2 our_texcoord;
public:
	TextrueShaderArgs(const vec2 &texcoord);
	virtual SArgsPtr interp(const SArgsPtr &other, float t) const noexcept override;
	virtual SArgsPtr interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept override;
	virtual void perspective_divide(float inverse_z) noexcept override;
};

class TextrueShader : public ShaderBase {
	Texture2d uniform_texture;
public:
	virtual void initialize() noexcept;
	virtual vec4 vertex(const Vertex &vertex, SArgsPtr &args) noexcept override;
	virtual bool fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept override;
};
