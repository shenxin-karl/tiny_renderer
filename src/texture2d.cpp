#include "common.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2d::Texture2d(const std::string &path) : image_info_ptr(Loader::load_image(path)) {
#ifdef _DEBUG
	texture_name = path;
#endif // _DEBUG
}

vec3 Texture2d::rgb(const vec2 &texcoord) const {
	float s = texcoord.s();
	float t = texcoord.t();
#ifdef _DEBUG
	if (image_info_ptr == nullptr) {
		std::cerr << texture_name << " texture: image_data is nullptr" << std::endl;
		return vec3(0);
	}
#endif // _DEBUG
	constexpr float inverse_255 = 1.f / 255.f;
	unsigned char *data = image_info_ptr->data;
	int width = image_info_ptr->width;
	int height = image_info_ptr->height;
	int y = static_cast<int>((1-t) * height);
	int x = static_cast<int>(s *width);
	int index = y * height * 3 + x * 3;
	return vec3{
		data[index]	* inverse_255,
		data[index+1] * inverse_255,
		data[index+2] * inverse_255,
	};
}

vec4 Texture2d::rgba(const vec2 &texcoord) const {
	float s = texcoord.s();
	float t = texcoord.t();
#ifdef _DEBUG
	if (image_info_ptr == nullptr) {
		std::cerr << texture_name << " texture: image_data is nullptr" << std::endl;
		return vec4(0);
	}
#endif // _DEBUG
	constexpr float inverse_255 = 1.f / 255.f;
	unsigned char *data = image_info_ptr->data;
	int width = image_info_ptr->width;
	int height = image_info_ptr->height;
	int y = static_cast<int>((1-t) * height);
	int x = static_cast<int>(s *width);
	int index = y * height * 4 + x * 4;
	return vec4 {
		data[index]	* inverse_255,
		data[index+1] * inverse_255,
		data[index+2] * inverse_255,
		data[index+3] * inverse_255,
	};
}

vec3 Texture2d::normal(const vec2 &texcoord) const {
	auto noraml = rgb(texcoord);
	noraml = normalized(noraml * 2.f - 1.f); 
	return noraml;
}
