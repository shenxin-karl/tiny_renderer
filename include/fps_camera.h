#pragma once

#undef near
#undef far

class FpsCamera : public CameraBase {
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
	float last_x = 0.f;
	float last_y = 0.f;
public:
	FpsCamera(vec3 _look_from, vec3 _look_up, float _fov, float _aspect, 
		   float _near, float _far, float _speek, float _sensitivtiy);

	const vec3 &get_look_from() const;
	const vec3 &get_look_up() const;
	const vec3 &get_look_at() const;
public:
	virtual void key_callback(Window::WindowKey key, float delte_time) override;
	virtual void mouse_callback(int x, int y) override;
	virtual void scroll_callback(float offset) override;
	virtual void frame_callback(int width, int height) override;
	virtual const mat4 &get_view() const override;
	virtual const mat4 &get_projection() const override;
private:
	void update_base_vec();
};