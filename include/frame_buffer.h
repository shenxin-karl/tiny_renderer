#pragma once

#undef far
#undef near

enum class FrameBufferType {
	ColorBuffer = 0x1, 
	DepthBuffer = 0x2,
};

constexpr FrameBufferType operator|(const FrameBufferType &lhs, const FrameBufferType &rhs) {
	return FrameBufferType(static_cast<int>(lhs) | static_cast<int>(rhs));
}

constexpr bool operator&(const FrameBufferType &lhs, const FrameBufferType &rhs) {
	return static_cast<int>(lhs) & static_cast<int>(rhs);
}



class FrameBuffer {
	using frame_color_t = std::array<unsigned char, 3>;
	using depth_color_t = unsigned char;
public:
	//std::vector<vec3>	frame_buffer;
	//std::vector<float>	depth_buffer;
	std::unique_ptr<frame_color_t[]>	frame_buffer;
	std::unique_ptr<depth_color_t[]>  depth_buffer;
	int	 width;
	int	 height;
	vec3 fill_color;
public:
	FrameBuffer(int _width, int _height);
	FrameBuffer(const FrameBuffer &) = delete;
	void set_color(const vec3 &point, const vec3 &color);
	int get_index(const vec2 &point) const;
	bool check_depth(const vec3 &point) const;
	void resize(int _width, int _height, bool refresh_buffer = false);
	void clear(FrameBufferType type);
	void clear_color(const vec3 &color);
	void save(const std::string &path, FrameBufferType flag) const;
	int get_width() const;
	int get_height() const;
	const unsigned char *get_frame_data() const noexcept;
	~FrameBuffer() = default;
	friend class Window;
};