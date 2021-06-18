#pragma once

class Texture2d {
public:
	Texture2d() = default;
	Texture2d(const std::string &path);
	Texture2d(const Texture2d &) = default;
	Texture2d(Texture2d &&) = default;
	Texture2d &operator=(const Texture2d &) = default;
	~Texture2d() = default;
	vec3 rgb(vec2 texcoord) const;
	vec4 rgba(vec2 texcoord) const;
	vec3 normal(vec2 texcoord) const;
	float channel(vec2 texcoord, int idx) const;
	explicit operator bool() const noexcept;
private:
	static constexpr float inverse_255 = 1.f / 255.f;
	std::tuple<unsigned char *, int> unpack_texcoord(vec2 texcoord) const noexcept;
private:
	std::shared_ptr<ImageInfo> image_info_ptr;
#ifdef _DEBUG
	std::string texture_name;
#endif // _DEBUG
};