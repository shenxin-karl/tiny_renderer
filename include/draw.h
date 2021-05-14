#pragma once

class Draw {
public:
	static void line(FrameBuffer &frame, const vec3 &start, const vec3 &last, const vec3 &color);
	static void triangle(FrameBuffer &frame, Shader &shader, std::array<Vertex &, 3> vertice);
	static vec3 barycentric_coord(vec2 point, const vec3 &v1, const vec3 &v2, const vec3 &v3);

	static inline float M_PI = std::acos(-1.f);
	static inline float radians(float angle);
	static mat4 viewport(int width, int height);
	static mat4 ortho(float fov, float aspect, float n, float f);
	static mat4 projection(float fov, float aspect, float n, float f);
	static mat4 view(vec3 look_from, vec3 look_up, vec3 look_at);
};