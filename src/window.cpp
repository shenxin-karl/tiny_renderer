#include "common.h"

Window::Window(int _width, int _height, const std::string &title) 
: should_be_close(false), window_hwnd(0), width(_width), height(_height) {

	HINSTANCE hins = GetModuleHandle(0);
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hins;
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;	
	wc.hCursor = nullptr;
	wc.lpfnWndProc = wnd_proc;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = "LYSM_class";

	if (!RegisterClassEx(&wc)) {
		std::cerr << "RegisterClassEx error: " << GetLastError() << std::endl;
		return;
	}

	window_hwnd = CreateWindowEx(
		WS_EX_TOPMOST, 
		"LYSM_class", 
		title.c_str(),
		WS_OVERLAPPEDWINDOW,
		0, 
		0, 
		width,
		height,
		0,
		0,
		hins,
		0
	);

	if (window_hwnd == 0) {
		std::cerr << "CreateWindowEx error:" << GetLastError() << std::endl;
		return;
	}

	hwnd_to_window.insert(std::make_pair(window_hwnd, this));
	UpdateWindow(window_hwnd);
	ShowWindow(window_hwnd, SW_SHOW);

	key_callback = [](Window *, WindowKey) {};
	mouse_callback = [](Window *, int, int) {};
	scroll_callback = [](Window *, float) {};
	frame_callback = [](Window *, int, int) {};
}

float Window::get_time() {
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<float> diff = end - start_time;
	return diff.count();
}

bool Window::window_should_be_close() const {
	return should_be_close;
}

void Window::set_window_should_be_close(bool flag) {
	should_be_close = flag;
}

void Window::poll_event() {
	MSG msg;
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 	// 从消息队列中删除一条消息
		DispatchMessage(&msg);
}

void Window::draw(const FrameBuffer &frame_buff) {
	if (frame_buff.width != width || frame_buff.height != height)
		return;

	std::unique_ptr<unsigned char[]> buffer(new unsigned char[width * height * 3]);
	for (size_t idx = 0; const vec3 &rgb : frame_buff.frame_buffer) {
		buffer[idx  ] = static_cast<unsigned char>(rgb.b() * 255.f);
		buffer[idx+1] = static_cast<unsigned char>(rgb.g() * 255.f);
		buffer[idx+2] = static_cast<unsigned char>(rgb.r() * 255.f);
		idx += 3;
	}

	int nx = width; // 图像的宽度
	int ny = height; // 图像的高度
	int channels = 3; // 3代表rgb三通道，4则表示还有A，Alpha透明通道

	BITMAPINFO bmi; // 创建一个位图信息数据
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nx;  // 指定位图宽度
	bmi.bmiHeader.biHeight = ny; // 指定位图高度
	bmi.bmiHeader.biPlanes = 1;  // 指定层数，位图有能是多维的
	bmi.bmiHeader.biBitCount = 24; // 指定每个像素的位数，这里是rgb，分别用char类型表示，共24位
								   // 如果你的帧数据是rgba，这里应制定32
	bmi.bmiHeader.biCompression = BI_RGB; // BI_RGB = 0，表示压缩程度，0是无损耗，图像质量高
	bmi.bmiHeader.biSizeImage = nx * ny * 3; // 图像的大小

	HDC hdc = GetDC(window_hwnd);
	StretchDIBits(hdc, 0, 0, bmi.bmiHeader.biWidth,
		bmi.bmiHeader.biHeight, 0, 0, bmi.bmiHeader.biWidth,
		bmi.bmiHeader.biHeight, buffer.get(), (BITMAPINFO *)&bmi.bmiHeader,
		DIB_RGB_COLORS, SRCCOPY);
}

Window::~Window() {
	if (window_hwnd == 0)
		return;

	hwnd_to_window.erase(window_hwnd);
	DestroyWindow(window_hwnd);
}

LRESULT CALLBACK Window::wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	HDC hdc;
	Window *window = nullptr;
	if (auto iter = hwnd_to_window.find(hwnd); iter != hwnd_to_window.end())
		window = iter->second;
	if (window == nullptr)
		return DefWindowProc(hwnd, msg, wparam, lparam);;

	switch (msg) {
	case WM_DESTROY:
	case WM_CLOSE: 
	{
		window->set_window_should_be_close(true);
		break;
	}
	case WM_PAINT:		// 窗口大小改变
	{			
		RECT rect;
		GetWindowRect(hwnd, &rect);
		window->width = rect.right - rect.left;
		window->height = rect.bottom - rect.top;
		window->frame_callback(window, window->width, window->height);
		break;
	}
	case WM_MOUSEMOVE:	// 鼠标移动
	{		
		hdc = GetDC(hwnd);		
		int x = LOWORD(lparam);	//鼠标的横坐标
		int y = HIWORD(lparam);	//鼠标的纵坐标
		window->mouse_callback(window, x, y);
		break;
	}
	case WM_KEYDOWN: 
	case WM_KEYUP: 
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN: 
	{
		static std::map<decltype(WM_KEYDOWN), WindowKey> key_event_map = {
			std::make_pair(WM_KEYUP, WindowKey::Froward),
			std::make_pair(WM_KEYDOWN, WindowKey::Backward),
			std::make_pair(WM_LBUTTONDOWN, WindowKey::Left),
			std::make_pair(WM_RBUTTONDOWN, WindowKey::Right),
		};
		if (auto iter = key_event_map.find(msg); iter != key_event_map.end())
			window->key_callback(window, iter->second);
		break;
	}
	case WM_MOUSEWHEEL: 
	{
		float offset = GET_WHEEL_DELTA_WPARAM(wparam) / (float)WHEEL_DELTA;
		window->scroll_callback(window, offset);
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

