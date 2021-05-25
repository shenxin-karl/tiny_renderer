#include "common.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2d::Texture2d(const std::string &path) {
	unsigned char *image_data = stbi_load(path.c_str(), &width, &height, &channel, 0);
	if (image_data == nullptr)
		return;

	data = std::shared_ptr<unsigned char>(image_data, RecycleBin());
#ifdef _DEBUG
	texture_name = path;
#endif // _DEBUG
}

vec3 Texture2d::rgb(const vec2 &texcoord) const {
	float s = texcoord.s();
	float t = texcoord.t();
	unsigned char *image_data = data.get();
#ifdef _DEBUG
	if (image_data == nullptr) {
		std::cerr << texture_name << " texture: image_data is nullptr" << std::endl;
		return vec3(0);
	}
#endif // _DEBUG
	int y = static_cast<int>((1-t) * height);
	int x = static_cast<int>(s * width);
	int index = y * width * 3 + x * 3;
	constexpr float inverse_255 = 1.f / 255.f;
	return vec3{
		image_data[index]	* inverse_255,
		image_data[index+1] * inverse_255,
		image_data[index+2] * inverse_255,
	};
}

vec4 Texture2d::rgba(const vec2 &texcoord) const {
	float s = texcoord.s();
	float t = texcoord.t();
	unsigned char *image_data = data.get();
#ifdef _DEBUG
	if (image_data == nullptr) {
		std::cerr << texture_name << " texture: image_data is nullptr" << std::endl;
		return vec4(0);
	}
#endif // _DEBUG
	int y = static_cast<int>((1-t) * height);
	int x = static_cast<int>(s * width);
	int index = y * width * 4 + x * 4;
	constexpr float inverse_255 = 1.f / 255.f;
	return vec4 {
		image_data[index]	* inverse_255,
		image_data[index+1] * inverse_255,
		image_data[index+2] * inverse_255,
		image_data[index+3] * inverse_255,
	};
}

vec3 Texture2d::normal(const vec2 &texcoord) const {
	auto noraml = rgb(texcoord);
	noraml = normalized(noraml * 2.f - 1.f); 
	return noraml;
}

void Texture2d::RecycleBin::operator()(unsigned char *data) const noexcept {
	if (data == nullptr)
		return;
	stbi_image_free(data);
}
