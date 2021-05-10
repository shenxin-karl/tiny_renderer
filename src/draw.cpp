#include "common.h"

using std::swap;
void Draw::line(FrameBuffer &frame, const vec3 &start, const vec3 &last, const vec3 &color) {
	int x = static_cast<int>(std::ceil(start.x()));
	int y = std::ceil(start.y());
	int sx = start.x() < last.x() ? 1 : -1;
	int sy = start.y() < last.y() ? 1 : -1;
	std::function<void()> long_step = [&]() { x += sx; };
	std::function<void()> short_step = [&]() { y += sy; };
	int nx = std::abs(std::floor(last.x()) - x);
	int ny = std::abs(std::floor(last.y()) - y);
	if (nx < ny)
		swap(long_step, short_step);

	int limit = std::max(nx, ny);
	int min = std::min(nx, ny);
	int delaty = min * 2;
	int middle = limit;
	float stepz = (last.z() - start.z()) / limit;
	float z = start.z();

	frame.set_color(vec3(x, y, z), color);
	for (int i = 0; i < limit; ++i) {
		if (delaty > middle) {
			long_step();
			middle += limit;
		}
		delaty += min;
		z += stepz;
		short_step();
		frame.set_color(vec3(x, y, 0), color);
	}
}