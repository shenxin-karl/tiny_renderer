#include "common.h"

constexpr int unpack_texcoord(vec2 texcoord, int width, int height, int channel) {
	int y = std::clamp(static_cast<int>((1 - texcoord.t()) * height), 0, height - 1);
	int x = std::clamp(static_cast<int>(texcoord.s() * width), 0, width - 1);
	int index = y * height * channel + x * channel;
	return index;
}

DepthBuffer::DepthBuffer(int _width, int _height) 
: width(_width), height(_height)
, buffer(_width * _height, 0.f) {

}

void DepthBuffer::clear() {
	std::fill(buffer.begin(), buffer.end(), 0.f);
}

void DepthBuffer::resize(int _width, int _height) {
	width = _width;
	height = _width;
	size_t count = static_cast<size_t>(width * height);
	buffer.resize(count);
}

bool DepthBuffer::check_depth(const vec3 &point) const noexcept {
	int x = static_cast<int>(point.x());
	int y = static_cast<int>(point.y());
	int index = y * width + x;
	return point.z() >= buffer[index];
}

float DepthBuffer::depth(vec2 texcoord) const noexcept {
	int index = unpack_texcoord(texcoord, width, height, 1);
	return buffer[index];
}

void DepthBuffer::set(int index, float z) noexcept {
	buffer[index] = z;
}

void DepthBuffer::save(const std::string &path) const {
	std::string filename = path + "_depth.ppm";
	std::fstream fos(filename, std::ios::out);
	if (!fos.is_open()) {
		std::cerr << "cann't open the file: " << filename << std::endl;
		return;
	}
	for (int y = height-1; y >= 0; --y) {
		for (int x = 0; x < width; ++x) {
			int index = y * width + x;
			float gray = buffer[index];
			unsigned char c = static_cast<unsigned char>(gray * 255.f);
			for (int i = 0; i < 3; ++i)
				fos << c;
		}
	}
	fos.close();
}

ColorBuffer::ColorBuffer(int _width, int _height, const vec3 &_fill_color)
: width(_width), height(_height) {
	clear_color(_fill_color);
	buffer.resize(width * height, fill_color);
}

void ColorBuffer::clear() {
	std::fill(buffer.begin(), buffer.end(), fill_color);
}

void ColorBuffer::resize(int _width, int _height) {
	width = _width;
	height = _height;
	size_t count = static_cast<size_t>(width * height);
	buffer.resize(count);
}

void ColorBuffer::clear_color(const vec3 &color) {
	fill_color = {
		static_cast<unsigned char>(color.r() * 255.f),
		static_cast<unsigned char>(color.g() * 255.f),
		static_cast<unsigned char>(color.b() * 255.f),
	};
}

void ColorBuffer::set(int index, const vec3 &color) {
	buffer[index] = {
		static_cast<unsigned char>(std::clamp(color.r(), 0.f, 1.f) * 255.f),
		static_cast<unsigned char>(std::clamp(color.g(), 0.f, 1.f) * 255.f),
		static_cast<unsigned char>(std::clamp(color.b(), 0.f, 1.f) * 255.f),
	};
}

vec3 ColorBuffer::rgb(vec2 texcoord) const noexcept {
	auto index = unpack_texcoord(texcoord, width, height, 1);
	const std::array<unsigned char, 3> &color = buffer[index];
	return {
		color[0] * inverse_255,
		color[1] * inverse_255,
		color[2] * inverse_255,
	};
}

vec3 ColorBuffer::normal(vec2 texcoord) const noexcept {
	return normalized(rgb(texcoord));
}

const unsigned char *ColorBuffer::data() const noexcept {
	static_assert(sizeof(fill_color) == sizeof(unsigned char) * 3);
	return reinterpret_cast<const unsigned char *>(buffer.data());
}

void ColorBuffer::save(const std::string &path) const {
	std::string filename = path + "_color.ppm";
	std::fstream fos(filename, std::ios::out);
	if (!fos.is_open()) {
		std::cerr << "cann't open the file: " << filename << std::endl;
		return;
	}

	fos << "P6\n" << width << " " << height << "\n255\n";
	size_t count = size_t(width) * size_t(height) * 3ul;
	fos.write(reinterpret_cast<const char *>(data()), count);
	fos.close();
}

FrameBuffer::FrameBuffer(int _width, int _height) 
: width(_width), height(_height) {
	color_buffer = std::make_shared<ColorBuffer>(_width, _height);
	depth_buffer = std::make_shared<DepthBuffer>(_width, _height);
	clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
}

void FrameBuffer::set_color(const vec3 &point, const vec3 &color) {
	int index = get_index(point.head<2>());
	color_buffer->set(index, color);
	depth_buffer->set(index, point.z());
}

int FrameBuffer::get_index(const vec2 &point) const {
	int x = static_cast<int>(point.x());
	int y = static_cast<int>(point.y());
	return (y * width) + x;
}

bool FrameBuffer::check_depth(const vec3 &point) const {
	return depth_buffer->check_depth(point);
}

void FrameBuffer::resize(int _width, int _height) {
	assert(_width > 0);
	assert(_height > 0);
	width = _width;
	height = _height;
	color_buffer->resize(width, height);
	depth_buffer->resize(width, height);
}

void FrameBuffer::clear(FrameBufferType type) {
	if (type & FrameBufferType::ColorBuffer)
		color_buffer->clear();
	if (type & FrameBufferType::DepthBuffer)
		depth_buffer->clear();
}

void FrameBuffer::clear_color(const vec3 &color) {
	color_buffer->clear_color(color);
}

void FrameBuffer::save(const std::string &path, FrameBufferType flag) const {
	if (flag & FrameBufferType::ColorBuffer)
		color_buffer->save(path);
	if (flag & FrameBufferType::DepthBuffer)
		depth_buffer->save(path);
}

int FrameBuffer::get_width() const {
	return width;
}

int FrameBuffer::get_height() const {
	return height;
}