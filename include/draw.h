#pragma once

class Draw {
public:
	static void line(FrameBuffer &frame, const vec3 &start, const vec3 &last, const vec3 &color);
	static void triangle(FrameBuffer &frame, ShaderBase &shader, std::array<VertexRes *, 3> vertice);
	static vec3 barycentric_coord(vec2 point, const vec3 &v1, const vec3 &v2, const vec3 &v3);

	static inline float M_PI = std::acos(-1.f);
	static float radians(float angle);
	static float random() noexcept;
	static mat4 ortho(float fov, float aspect, float n, float f);
	static mat4 projection(float fov, float aspect, float n, float f);
	static mat4 view(vec3 look_from, vec3 look_up, vec3 look_at);
	static mat4 rotate_y(float angle);
	static mat4 rotate_x(float angle);
	static bool plane_cutting(std::vector<VertexRes> &out_vertices);
private:
	constexpr static float plane_w_limit = -0.000001f;
	static bool outside_left_plane(float x, float w);
	static bool outside_right_plane(float x, float w);
	static bool outside_top_plane(float y, float w);
	static bool outside_bottom_plane(float y, float w);
	static bool outside_near_plane(float z, float w);
	static bool outside_far_plane(float z, float w);
	static bool outside_w_plane(float, float w);
	static VertexRes interp_vertex_res(const VertexRes &start, const VertexRes &last, float t);

	template<size_t I>
	static float get_negative_plane_ratio(const vec4 &start, const vec4 &last) {
		float t1 = start[I] + start.w();
		float t2 = last[I] + last.w();
		float res = t1 / (t1 - t2);
		return res;
	}

	template<size_t I>
	static float get_positive_plane_ratio(const vec4 &start, const vec4 &last) {
		float t1 = start[I] - start.w();
		float t2 = last[I] - last.w();
		float res = t1 / (t1 - t2);
		return res;
	}

	inline static float get_w_plane_ratio(const vec4 &start, const vec4 &last) {
		float t1 = plane_w_limit - start.w();
		float t2 = last.w() - start.w();
		float res = t1 / t2;
		return res;
	}
};