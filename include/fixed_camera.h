#pragma once

class FixedCamera : public CameraBase {
	mat4 view;
	mat4 projection;
public:
	FixedCamera(const vec3 &_look_from, const vec3 &_look_at, const vec3 &_look_up,
		float _fov, float _aspect, float _near, float _far);
	virtual void key_callback(Window::WindowKey key, float delte_time) override;
	virtual void mouse_callback(int x, int y) override;
	virtual void scroll_callback(float offset) override;
	virtual void frame_callback(int width, int height) override;
	virtual const mat4 &get_view() const override;
	virtual const mat4 &get_projection() const override;
};