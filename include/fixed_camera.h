#pragma once

class FixedCamera : public CameraBase {
	mat4 view;
	mat4 projection;
public:
	using CameraBase::CameraBase;
	virtual void key_callback(Window::WindowKey key, float delte_time) override;
	virtual void mouse_callback(int x, int y) override;
	virtual void scroll_callback(float offset) override;
	virtual void frame_callback(int width, int height) override;
	virtual const mat4 &get_view() const override;
	virtual const mat4 &get_projection() const override;
};