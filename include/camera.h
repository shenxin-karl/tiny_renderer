#pragma once

#undef near
#undef far

class Camera {
	vec3  look_from;
	vec3  look_at;
	vec3  look_up;
	vec3  world_up;
	vec3  world_right;
	float fov;
	float aspect;
	float near;
	float far;
	float pitch;
	float yaw;
	float speek;
	float sensitivity;
	mat4  view;
	mat4  projection;
public:
	Camera(vec3 _look_from, vec3 _look_up, float _fov, float _aspect, 
		   float _near, float _far, float _speek, float _sensitivtiy);
	const mat4 &get_view() const;
	const mat4 &get_projection() const;
	const vec3 &get_look_from() const;
	const vec3 &get_look_up() const;
	const vec3 &get_look_at() const;
public:
	void key_callback(Window::WindowKey key, int delte_time);
	void mouse_callback(int x, int y);
	void scroll_callback(float offset);
	void frame_callback(int width, int height);
private:
	void update_base_vec();
};