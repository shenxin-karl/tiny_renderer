#pragma once


class TextureCube {
	std::array<Texture2d, 6> textures;
public:
	/* The texture order is:
	 * @right
	 * @left
	 * @top
	 * @bottom
	 * @back
	 * @front
	 */
	TextureCube(const std::array<std::string, 6> &faces);
	vec3 rgb(const vec3 &direction) const noexcept;
	vec4 rgba(const vec3 &direction) const noexcept;
	TextureCube &operator=(const TextureCube &other) = default;
private:
	std::tuple<int, vec2> calc_texture_index(const vec3 &direction) const noexcept;
private:
	friend class SkyboxShader;
	TextureCube() = default;
};