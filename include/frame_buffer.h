#pragma once

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

constexpr int unpack_texcoord(vec2 texcoord, int width, int height, int channel);


class DepthBuffer {
	std::vector<float>	buffer;
	int	width;
	int	height;
public:
	DepthBuffer(int _width, int _height);
	DepthBuffer(const DepthBuffer &) = delete;
	void clear();
	void resize(int _width, int _height);
	bool check_depth(const vec3 &point) const noexcept;
	float depth(vec2 texcoord) const noexcept;
	void set(int index, float z) noexcept;
};


class ColorBuffer {
	constexpr static float inverse_255 = 1.f / 255.f;
	std::vector<std::array<unsigned char, 3>> buffer;
	std::array<unsigned char, 3> fill_color;
	int  width;
	int	 hegiht;
public:
	ColorBuffer(int _width, int _height, const vec3 &_fill_color = vec3{});
	ColorBuffer(const ColorBuffer &) = delete;
	void clear();
	void resize(int _width, int _height);
	void clear_color(const vec3 &color);
	void set(int index, const vec3 &color);
	vec3 rgb(vec2 texcoord) const noexcept;
	vec3 normal(vec2 texcoord) const noexcept;
	const unsigned char *data() const noexcept;
};


class FrameBuffer {
	friend class Window;
	std::shared_ptr<ColorBuffer> color_buffer;
	std::shared_ptr<DepthBuffer> depth_buffer;
	int width;
	int height;
public:
	FrameBuffer(int _width, int _height);
	FrameBuffer(const FrameBuffer &) = delete;
	void set_color(const vec3 &point, const vec3 &color);
	int get_index(const vec2 &point) const;
	bool check_depth(const vec3 &point) const;
	void resize(int _width, int _height);
	void clear(FrameBufferType type);
	void clear_color(const vec3 &color);
	void save(const std::string &path, FrameBufferType flag) const;
	int get_width() const;
	int get_height() const;
	~FrameBuffer() = default;
};