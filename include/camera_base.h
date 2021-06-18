#pragma once

class CameraBase {
protected:
	vec3  look_from;
	vec3  look_at;
	vec3  look_up;
	float fov;
	float aspect;
	float near;
	float far;
public:
	CameraBase(const vec3 &_look_from, const vec3 &_look_at, const vec3 &_look_up, 
			   float _fov, float _aspect, float _near, float _far);
	virtual ~CameraBase() = default;
	virtual void key_callback(Window::WindowKey key, float delte_time) = 0;
	virtual void mouse_callback(int x, int y) = 0;
	virtual void scroll_callback(float offset) = 0;
	virtual void frame_callback(int width, int height) = 0;
	virtual const mat4 &get_view() const = 0;
	virtual const mat4 &get_projection() const = 0;
public:
	const vec3 &get_look_from() const;
	const vec3 &get_look_up() const;
	const vec3 &get_look_at() const;
};