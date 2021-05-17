#include "common.h"

Camera::Camera(vec3 _look_from, vec3 _look_up, float _fov, float _aspect, 
			   float _near, float _far, float _speek, float _sensitivtiy) 
: look_from(_look_from), world_up(_look_up), fov(_fov), aspect(_aspect)
, near(_near), far(_far), pitch(0), yaw(0), speek(_speek), sensitivity(_sensitivtiy) {
	update_base_vec();
}

const mat4 &Camera::get_view() const {
	return view;
}

const mat4 &Camera::get_projection() const {
	return projection;
}

const vec3 &Camera::get_look_from() const {
	return look_from;
}

const vec3 &Camera::get_look_up() const {
	return look_up;
}

const vec3 &Camera::get_look_at() const {
	return look_at;
}

void Camera::key_callback(Window::WindowKey key, int delta_time) {
	switch (key) {
	case Window::WindowKey::Froward:
		look_from += look_at * (delta_time * speek);
		break;
	case Window::WindowKey::Backward:
		look_from -= look_at * (delta_time * speek);
		break;
	case Window::WindowKey::Left:
		look_from -= world_right * (delta_time * speek);
		break;
	case Window::WindowKey::Right:
		look_from += world_right * (delta_time * speek);
		break;
	default:
		return;
	}
	update_base_vec();
}

void Camera::mouse_callback(int x, int y) {
	static float last_x = 0.f;
	static float last_y = 0.f;
	if (last_x == 0.f && last_y == 0.f) {
		last_x = static_cast<float>(x);
		last_y = static_cast<float>(y);
		return;
	}

	float offset_x = x - last_x;
	float offset_y = y - last_y;
	pitch = std::clamp(pitch + (offset_y * sensitivity), -89.f, 89.f);
	yaw = yaw + offset_x * sensitivity;
	update_base_vec();
}

void Camera::scroll_callback(float offset) {
	fov = std::clamp(fov + offset, 0.f, 89.9f);
	projection = Draw::projection(fov, speek, near, far);
}

void Camera::frame_callback(int width, int height) {
	aspect = static_cast<float>(width) / static_cast<float>(height);
	projection = Draw::projection(fov, aspect, near, far);
}

void Camera::update_base_vec() {
	vec3 offset;
	offset.y() = std::sin(Draw::radians(pitch));
	offset.x() = std::cos(Draw::radians(pitch)) * std::cos(Draw::radians(yaw));
	offset.z() = std::cos(Draw::radians(pitch)) * std::sin(Draw::radians(yaw));
	look_at = normalized(offset);
	world_right = normalized(cross(look_at, world_up));
	look_up = normalized(cross(world_right, look_at));
	view = Draw::view(look_from, look_up, look_from + look_up);
}
