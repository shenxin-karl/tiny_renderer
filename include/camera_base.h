#pragma once

class CameraBase {
public:
	virtual ~CameraBase() = default;
	virtual void key_callback(Window::WindowKey key, int delte_time) = 0;
	virtual void mouse_callback(int x, int y) = 0;
	virtual void scroll_callback(float offset) = 0;
	virtual void frame_callback(int width, int height) = 0;
};