#include "common.h"

FrameBuffer::FrameBuffer(int _width, int _height) 
: width(_width), height(_height), frame_buffer(_width * _height)
, depth_buffer(_width * _height, std::numeric_limits<float>::max()), fill_color(0) {

}

void FrameBuffer::set_color(const vec3 &point, const vec3 &color) {
	int x = static_cast<int>(std::round(point.x()));
	int y = static_cast<int>(std::round(point.y()));
	int index = (y * width) + x;
	if (point.z() < depth_buffer[index]) {
		depth_buffer[index] = point.z();
		frame_buffer[index] = color;
	}
}

void FrameBuffer::resize(int _width, int _height) {
	assert(_width > 0);
	assert(_height > 0);
	width = _width;
	height = _height;
	frame_buffer.resize(width * height);
	depth_buffer.resize(width * height);
	clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
}

void FrameBuffer::clear(FrameBufferType type) {
	if (type & FrameBufferType::ColorBuffer)
		std::fill(frame_buffer.begin(), frame_buffer.end(), fill_color);
	if (type & FrameBufferType::DepthBuffer)
		std::fill(depth_buffer.begin(), depth_buffer.end(), std::numeric_limits<float>::max());
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
