#pragma once

class Texture2d {
public:
	Texture2d(const std::string &path);
	Texture2d(const Texture2d &) = default;
	Texture2d(Texture2d &&) = default;
	Texture2d &operator=(const Texture2d &) = default;
	~Texture2d() = default;

	struct RecycleBin {
		void operator()(unsigned char *data) const noexcept;
	};

	vec3 texture(vec2 texcoord) const;
	vec3 texture(float s, float t) const;
private:
	int width;
	int	height;
	int channel;
	std::shared_ptr<unsigned char>	data;
#ifdef _DEBUG
	std::string texture_name;
#endif // _DEBUG
};