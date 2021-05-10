#include "common.h"

FrameBuffer::FrameBuffer(int _width, int _height) 
: width(_width), height(_height), frame_buffer(_width * _height)
, depth_buffer(_width * _height, std::numeric_limits<float>::max()), fill_color(0) {

}

void FrameBuffer::set_color(const vec3 &point, const vec3 &color) {
	int x = static_cast<int>(std::round(point.x()));
	int y = static_cast<int>(std::round(point.y()));
#if _DEBUG
	if (x < 0 || x >= width || y < 0 || y >= height)
		return;
#endif
	int index = (y * height) + x;
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
