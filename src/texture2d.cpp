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
	int channel = image_info_ptr->channel;
	int y = std::clamp(static_cast<int>((1 - t) * height), 0, height-1);
	int x = std::clamp(static_cast<int>(s * width), 0, width-1);
	int index = y * height * channel + x * channel;
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
	int channel = image_info_ptr->channel;
	int y = std::clamp(static_cast<int>((1 - t) * height), 0, height - 1);
	int x = std::clamp(static_cast<int>(s * width), 0, width - 1);
	int index = y * height * channel + x * channel;
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

Texture2d::operator bool() const noexcept {
	return image_info_ptr != nullptr;
}
