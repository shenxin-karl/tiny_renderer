#pragma once

class FrameBuffer;
class Window {
public:
	enum class WindowKey {
		Froward,
		Backward,
		Left,
		Right,
	};
private:
	bool	should_be_close;
	HWND	window_hwnd;
	int		width;
	int		height;
public:
	std::function<void(Window *, WindowKey)> key_callback;			// ���̻ص�
	std::function<void(Window *, int, int)>  mouse_callback;		// ���ص�
	std::function<void(Window *, float)>	 scroll_callback;		// ��껬�ֻص�
	std::function<void(Window *, int, int)>	 frame_callback;		// �޸Ĵ��ڴ�С�ص�
public:
	Window(int _width, int _height, const std::string &_title);
	float get_time();
	bool window_should_be_close() const;
	void set_window_should_be_close(bool flag);
	void poll_event();
	void draw(const FrameBuffer &frame_buff);
	~Window();
	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public:
	static inline std::map<HWND, Window *> hwnd_to_window;
	static inline auto start_time = std::chrono::steady_clock::now();
};
