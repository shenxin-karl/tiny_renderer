#include "common.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2d::Texture2d(const std::string &path) : image_info_ptr(Loader::load_image(path)) {
#ifdef _DEBUG
	texture_name = path;
#endif // _DEBUG
}

vec3 Texture2d::rgb(vec2 texcoord) const {
	auto &&[data, index] = unpack_texcoord(texcoord);
	return {
		data[index+0] * inverse_255,
		data[index+1] * inverse_255,
		data[index+2] * inverse_255,
	};
}

vec4 Texture2d::rgba(vec2 texcoord) const {
	auto &&[data, index] = unpack_texcoord(texcoord);
	return {
		data[index+0] * inverse_255,
		data[index+1] * inverse_255,
		data[index+2] * inverse_255,
		data[index+3] * inverse_255,
	};
}

vec3 Texture2d::normal(vec2 texcoord) const {
	auto noraml = rgb(texcoord);
	noraml = normalized(noraml * 2.f - 1.f); 
	return noraml;
}

float Texture2d::channel(vec2 texcoord, int idx) const {
	auto &&[data, index] = unpack_texcoord(texcoord);
	return data[index + idx] * inverse_255;
}

Texture2d::operator bool() const noexcept {
	return image_info_ptr != nullptr;
}

std::tuple<unsigned char *, int> Texture2d::unpack_texcoord(vec2 texcoord) const noexcept {
	static unsigned char default_color[4] = { 0 };
	if (image_info_ptr == nullptr) {
		assert(false);
		return { default_color, 0 };
	}

	int width = image_info_ptr->width;
	int height = image_info_ptr->height;
	int channel = image_info_ptr->channel;
	int y = std::clamp(static_cast<int>((1.f - texcoord.t()) * height), 0, height-1);
	int x = std::clamp(static_cast<int>(texcoord.s() * width), 0, width-1);
	int index = (y * width * channel) + (x * channel);
	assert(index >= 0 && index < (width * height * channel));
	return { image_info_ptr->data, index };
}
