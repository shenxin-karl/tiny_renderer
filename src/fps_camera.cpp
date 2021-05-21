#include "common.h"

FpsCamera::FpsCamera(vec3 _look_from, vec3 _look_up, float _fov, float _aspect, 
			   float _near, float _far, float _speek, float _sensitivtiy) 
: CameraBase(_look_from, vec3(0), _look_up, _fov, _aspect, _near, _far)
, world_up(_look_up), pitch(0), yaw(0), speed(_speek), sensitivity(_sensitivtiy) {

	projection = Draw::projection(fov, aspect, near, far);
	update_base_vec();
}

void FpsCamera::key_callback(Window::WindowKey key, float delta_time) {
	float cur_speed = speed * delta_time;
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
	view = Draw::view(look_from, look_up, look_from + look_at);
}

void FpsCamera::mouse_callback(int x, int y) {
	if (last_x == 0.f && last_y == 0.f) {
		last_x = static_cast<float>(x);
		last_y = static_cast<float>(y);
		return;
	}

	float offset_x = x - last_x;
	float offset_y = last_y - y;
	last_x = static_cast<float>(x);
	last_y = static_cast<float>(y);
	pitch = std::clamp(pitch + (offset_y * sensitivity), -89.f, 89.f);
	yaw = yaw + offset_x * sensitivity;
	update_base_vec();
}

void FpsCamera::scroll_callback(float offset) {
	fov = std::clamp(fov + offset, 0.1f, 45.f);
	std::cout << "fov " << fov << std::endl;
	projection = Draw::projection(fov, aspect, near, far);
}

void FpsCamera::frame_callback(int width, int height) {
	aspect = static_cast<float>(width) / static_cast<float>(height);
	std::cout << "aspect " << aspect << std::endl;
	projection = Draw::projection(fov, aspect, near, far);
}

const mat4 &FpsCamera::get_view() const {
	return view;
}

const mat4 &FpsCamera::get_projection() const {
	return projection;
	//static mat4 mat;
	//mat = Draw::ortho(fov, aspect, near, far);
	//return mat;
}

void FpsCamera::update_base_vec() {
	vec3 offset;
	offset.y() = std::sin(Draw::radians(pitch));
	offset.x() = std::cos(Draw::radians(pitch)) * std::cos(Draw::radians(yaw));
	offset.z() = std::cos(Draw::radians(pitch)) * std::sin(Draw::radians(yaw));
	look_at = normalized(-offset);
	world_right = normalized(cross(world_up, look_at));
	look_up = normalized(cross(look_at, world_right));
	view = Draw::view(look_from, look_up, look_from + look_at);
}
