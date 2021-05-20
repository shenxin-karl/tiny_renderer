#include "common.h"

void FixedCamera::key_callback(Window::WindowKey key, float delte_time) {

}

void FixedCamera::mouse_callback(int x, int y) {

}

void FixedCamera::scroll_callback(float offset) {
	std::cout << "fov " << fov << std::endl;
	fov = std::clamp(fov - offset, 0.1f, 89.f);
	view = Draw::view(look_from, look_up, look_at);
}

void FixedCamera::frame_callback(int width, int height) {
	aspect = static_cast<float>(width) / static_cast<float>(height);
	projection = Draw::ortho(fov, aspect, near, far);
}

const mat4 &FixedCamera::get_view() const {
	return view;
}

const mat4 &FixedCamera::get_projection() const {
	return projection;
}

