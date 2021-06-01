#include "common.h"

//#define USE_ORTHO_MATRIX

FixedCamera::FixedCamera(const vec3 &_look_from, const vec3 &_look_at, const vec3 &_look_up,
	float _fov, float _aspect, float _near, float _far) : CameraBase(_look_from, _look_at, _look_up, _fov, _aspect, _near, _far) {
	view = Draw::view(look_from, look_up, look_at);
#ifndef USE_ORTHO_MATRIX
	projection = Draw::projection(fov, aspect, near, far);
#else
	projection = Draw::ortho(fov, aspect, near, far);
#endif // !USE_ORTHO_MATRIX
}

void FixedCamera::key_callback(Window::WindowKey key, float delte_time) {
	constexpr vec3 world_right = { 1, 0, 0 };
	float cur_speed = 50 * delte_time;
	switch (key) {
	case Window::WindowKey::Froward:
		look_from += look_at * cur_speed;
		break;
	case Window::WindowKey::Backward:
		look_from -= look_at * cur_speed;
		break;
	case Window::WindowKey::Left:
		look_from -= world_right * cur_speed;
		break;
	case Window::WindowKey::Right:
		look_from += world_right * cur_speed;
		break;
	default:
		return;
	}
	std::cout << "look_from: " << look_from << std::endl;
	view = Draw::view(look_from, look_up, look_at);
}

void FixedCamera::mouse_callback(int x, int y) {
}

void FixedCamera::scroll_callback(float offset) {
	fov = std::clamp(fov - (offset * 0.2f), 0.1f, 89.f);
	std::cout << "fov: " << fov << std::endl;
#ifndef USE_ORTHO_MATRIX
	projection = Draw::projection(fov, aspect, near, far);
#else
	projection = Draw::ortho(fov, aspect, near, far);
#endif // !USE_ORTHO_MATRIX
}

void FixedCamera::frame_callback(int width, int height) {
	aspect = static_cast<float>(width) / static_cast<float>(height);
	view = Draw::view(look_from, look_up, look_at);
#ifndef USE_ORTHO_MATRIX
	projection = Draw::projection(fov, aspect, near, far);
#else
	projection = Draw::ortho(fov, aspect, near, far);
#endif // !USE_ORTHO_MATRIX
}

const mat4 &FixedCamera::get_view() const {
	return view;
}

const mat4 &FixedCamera::get_projection() const {
	return projection;
}

