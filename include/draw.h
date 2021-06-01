#pragma once

class Draw {
public:
	static void line(FrameBuffer &frame, const vec3 &start, const vec3 &last, const vec3 &color);
	static void triangle(FrameBuffer &frame, ShaderBase &shader, std::array<Vertex *, 3> vertice);
	static vec3 barycentric_coord(vec2 point, const vec3 &v1, const vec3 &v2, const vec3 &v3);

	static inline float M_PI = std::acos(-1.f);
	static float radians(float angle);
	static float random() noexcept;
	static mat4 ortho(float fov, float aspect, float n, float f);
	static mat4 projection(float fov, float aspect, float n, float f);
	static mat4 view(vec3 look_from, vec3 look_up, vec3 look_at);
	static mat4 rotate_y(float angle);
	static mat4 rotate_x(float angle);
	static int plane_cutting(std::vector<Vertex> &vertices, std::vector<int> &indices);
private:
	static int plane_cutting_triangle(std::vector<Vertex> &vertices, std::vector<int> &indices,
									  std::span<int, 3> triangle_view, size_t plane_idx,
									  bool (*outside_func)(float, float), bool symbol);

	static Vertex interp_vertex(const Vertex &start, const Vertex &last, size_t plane_idx, bool symbol);
	static bool outside_left_plane (float x, float w);
	static bool outside_right_plane(float x, float w);
	static bool outside_top_plane(float y, float w);
	static bool outside_bottom_plane(float y, float w);
	static bool outside_back_plane(float z, float w);
	static bool outside_front_plane(float z, float w);
	static bool outside_w_plane(float f, float w);
};