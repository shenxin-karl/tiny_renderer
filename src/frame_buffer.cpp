#include "common.h"

constexpr float zbuffer_fill_value = -std::numeric_limits<float>::max();
FrameBuffer::FrameBuffer(int _width, int _height)
: width(_width), height(_height), frame_buffer(size_t(_width) * _height)
, depth_buffer(size_t(_width) * _height, zbuffer_fill_value), fill_color(0) {

}

void FrameBuffer::set_color(const vec3 &point, const vec3 &color) {
	int index = get_index(point.head<2>());
	frame_buffer[index] = color;
	depth_buffer[index] = point.z();
}

int FrameBuffer::get_index(const vec2 &point) const {
	int x = static_cast<int>(point.x());
	int y = static_cast<int>(point.y());
	return (y * width) + x;
}

bool FrameBuffer::check_depth(const vec3 &point) const {
	int index = get_index(point.head<2>());
	return point.z() > depth_buffer[index];
}

void FrameBuffer::resize(int _width, int _height, bool refresh_buffer) {
	assert(_width > 0);
	assert(_height > 0);
	width = _width;
	height = _height;
	size_t new_size = static_cast<size_t>(width) * static_cast<size_t>(height);
	frame_buffer.resize(new_size);
	depth_buffer.resize(new_size);
	if (refresh_buffer)
		clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
}

void FrameBuffer::clear(FrameBufferType type) {
	if (type & FrameBufferType::ColorBuffer)
		std::fill(frame_buffer.begin(), frame_buffer.end(), fill_color);
	if (type & FrameBufferType::DepthBuffer)
		std::fill(depth_buffer.begin(), depth_buffer.end(), zbuffer_fill_value);
}

void FrameBuffer::clear_color(const vec3 &color) {
	fill_color = color;
}

void FrameBuffer::save(const std::string &path, FrameBufferType flag) const {
	auto save_frame = [&]() {
		if (!(flag & FrameBufferType::ColorBuffer)) 
			return;

		std::string file_name = path + ".frame.ppm";
		std::fstream stream(file_name, std::ios::binary | std::ios::out);
		if (!stream.is_open()) {
			std::cout << "failed open " << file_name << std::endl;
			return;
		}
		
		int index = 0;
		stream << "P6\n" << width << " " << height << "\n255\n";
		for (int x = 0; x < width; ++x) {
			for (int y = height-1; y >= 0; --y) {
				const vec3 &vec = frame_buffer[index++];
				stream << static_cast<unsigned char>(vec.r() * 255)
					   << static_cast<unsigned char>(vec.g() * 255)
					   << static_cast<unsigned char>(vec.b() * 255);
			}
		}
		stream.close();
	};
	save_frame();
}

int FrameBuffer::get_width() const {
	return width;
}

int FrameBuffer::get_height() const {
	return height;
}

const float *FrameBuffer::get_frame_data() const noexcept {
	return reinterpret_cast<const float *>(frame_buffer.data());
}
