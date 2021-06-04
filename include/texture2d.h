#pragma once

struct ImageInfo;
class Texture2d {
public:
	Texture2d(const std::string &path);
	Texture2d(const Texture2d &) = default;
	Texture2d(Texture2d &&) = default;
	Texture2d &operator=(const Texture2d &) = default;
	~Texture2d() = default;
	vec3 rgb(const vec2 &texcoord) const;
	vec4 rgba(const vec2 &texcoord) const;
	vec3 normal(const vec2 &texcoord) const;
	explicit operator bool() const noexcept;
private:
	std::shared_ptr<ImageInfo> image_info_ptr;
#ifdef _DEBUG
	std::string texture_name;
#endif // _DEBUG
};