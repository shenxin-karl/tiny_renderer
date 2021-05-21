#include "common.h"

Window::Window(int _width, int _height, const std::string &title) 
: should_be_close(false), window_hwnd(0), width(_width), height(_height)
, keys{false}, hdc(nullptr) {

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
#ifdef _DEBUG1
		WS_ICONIC,
#else
		WS_EX_TOPMOST,
#endif // _DEBUG
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
	hdc = GetDC(window_hwnd);
	assert(hdc != nullptr);

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
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 	// ����Ϣ������ɾ��һ����Ϣ
		DispatchMessage(&msg);

	for (int i = 0; i < static_cast<int>(WindowKey::MaxCount); ++i) {
		if (!keys[i]) 
			continue;

		keys[i] = false;
		WindowKey key = static_cast<WindowKey>(i);
		key_callback(this, key);
	}
}

void Window::draw(const FrameBuffer &frame_buff) {
	if (frame_buff.width != width || frame_buff.height != height)
		return;

	size_t size = size_t(width) * size_t(height) * 3ul;
	std::unique_ptr<unsigned char[]> buffer(new unsigned char[size]);
	for (size_t idx = 0; const vec3 &rgb : frame_buff.frame_buffer) {
		buffer[idx  ] = static_cast<unsigned char>(rgb.b() * 255.f);
		buffer[idx+1] = static_cast<unsigned char>(rgb.g() * 255.f);
		buffer[idx+2] = static_cast<unsigned char>(rgb.r() * 255.f);
		idx += 3;
	}

	int nx = width; // ͼ��Ŀ��
	int ny = height; // ͼ��ĸ߶�
	int channels = 3; // 3����rgb��ͨ����4���ʾ����A��Alpha͸��ͨ��

	BITMAPINFO bmi; // ����һ��λͼ��Ϣ����
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nx;  // ָ��λͼ���
	bmi.bmiHeader.biHeight = ny; // ָ��λͼ�߶�
	bmi.bmiHeader.biPlanes = 1;  // ָ��������λͼ�����Ƕ�ά��
	bmi.bmiHeader.biBitCount = 24; // ָ��ÿ�����ص�λ����������rgb���ֱ���char���ͱ�ʾ����24λ
								   // ������֡������rgba������Ӧ�ƶ�32
	bmi.bmiHeader.biCompression = BI_RGB; // BI_RGB = 0����ʾѹ���̶ȣ�0������ģ�ͼ��������
	bmi.bmiHeader.biSizeImage = nx * ny * 3; // ͼ��Ĵ�С

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
	case WM_PAINT:		// ���ڴ�С�ı�
	{			
		RECT rect;
		GetWindowRect(hwnd, &rect);
		window->width = rect.right - rect.left;
		window->height = rect.bottom - rect.top;
		window->frame_callback(window, window->width, window->height);
		break;
	}
	case WM_MOUSEMOVE:	// ����ƶ�
	{		
		hdc = GetDC(hwnd);		
		int x = LOWORD(lparam);	//���ĺ�����
		int y = HIWORD(lparam);	//����������
		window->mouse_callback(window, x, y);
		break;
	}
	case WM_KEYUP: 
	{
		window->process_key_input(wparam, false);
		break;
	}
	case WM_KEYDOWN: 
	{
		window->process_key_input(wparam, true);
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

std::map<WPARAM, Window::WindowKey> Window::standard_key_to_window_key = {
	std::make_pair<WPARAM, WindowKey>(0x57, WindowKey::Froward),	// 'W'
	std::make_pair<WPARAM, WindowKey>(0x53, WindowKey::Backward),	// 'S'
	std::make_pair<WPARAM, WindowKey>(0x41, WindowKey::Left),		// 'A'
	std::make_pair<WPARAM, WindowKey>(0x44, WindowKey::Right),		// 'D'
};

void Window::process_key_input(WPARAM param, bool pressed) {
	if (param == VK_ESCAPE) {
		set_window_should_be_close(true);
		return;
	}
	if (auto iter = standard_key_to_window_key.find(param); iter != standard_key_to_window_key.end()) {
		int index = static_cast<int>(iter->second);
		keys[index] = pressed;
	}
}
