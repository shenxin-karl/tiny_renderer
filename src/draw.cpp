#include "common.h"

using std::swap;
void Draw::line(FrameBuffer &frame, const vec3 &start, const vec3 &last, const vec3 &color) {
	int x = static_cast<int>(std::ceil(start.x()));
	int y = static_cast<int>(std::ceil(start.y()));
	int sx = start.x() < last.x() ? 1 : -1;
	int sy = start.y() < last.y() ? 1 : -1;
	std::function<void()> long_step = [&]() { x += sx; };
	std::function<void()> short_step = [&]() { y += sy; };
	int nx = static_cast<int>(std::abs(std::floor(last.x()) - x));
	int ny = static_cast<int>(std::abs(std::floor(last.y()) - y));
	if (nx < ny)
		swap(long_step, short_step);

	int limit = std::max(nx, ny);
	int min = std::min(nx, ny);
	int delaty = min * 2;
	int middle = limit;

	frame.set_color(vec3(float(x), float(y), 0), color);
	for (int i = 0; i < limit; ++i) {
		if (delaty > middle) {
			short_step();
			middle += limit;
		}
		delaty += min;
		long_step();
		auto point = vec3(float(x), float(y), 0.f);
		frame.set_color(point, color);
	}
}

// Vertex 的所有属性都是除 w 的. 在 Shader::fragment 函数里面 透视矫正插值
void Draw::triangle(FrameBuffer &frame, Shader &shader, std::array<Vertex *, 3> vertice) {
	vec2 bboxmin(std::numeric_limits<float>::max());
	vec2 bboxmax(std::numeric_limits<float>::min());
	for (const Vertex *vertex_ptr : vertice) {
		for (int i = 0; i < 2; ++i) {
			bboxmin[i] = std::min(bboxmin[i], vertex_ptr->position[i]);
			bboxmax[i] = std::max(bboxmax[i], vertex_ptr->position[i]);
		}
	}

	const vec3 &vertex_w = shader.vertex_w;
	int minx = static_cast<int>(std::ceil(bboxmin.x()));
	int miny = static_cast<int>(std::ceil(bboxmin.y()));
	int maxx = static_cast<int>(std::floor(bboxmax.x()));
	int maxy = static_cast<int>(std::floor(bboxmax.y()));
	for (int x = minx; x < maxx; ++x) {
		for (int y = miny; y < maxy; ++y) {
			const vec3 &v1 = vertice[0]->position;
			const vec3 &v2 = vertice[1]->position;
			const vec3 &v3 = vertice[2]->position;
			vec3 coords = barycentric_coord(vec2(float(x), float(y)), v1, v2, v3);
			if (coords[0] < 0.f || coords[1] < 0.f || coords[2] < 0.f)
				continue;

			vec3 color;
			shader.coords = coords;
			if (shader.fragment(vertice, color)) {
				float z1 = coords[0] * v1.z() * vertex_w[0];
				float z2 = coords[1] * v2.z() * vertex_w[1];
				float z3 = coords[2] * v3.z() * vertex_w[2];
				float depth = z1 + z2 + z3;
				frame.set_color(vec3(float(x), float(y), depth), color);
			}
		}
	}
}

vec3 Draw::barycentric_coord(vec2 point, const vec3 &v1, const vec3 &v2, const vec3 &v3) {
	constexpr auto create_line_func = [](const vec3 &p1, const vec3 &p2) {
		float x0 = p1.x();
		float x1 = p2.x();
		float y0 = p1.y();
		float y1 = p2.y();
		return [=](const vec2 &p) {
			return (y1-y0)*p.x() + (x0-x1)*p.y() + y0*x1 - x0*y1;
		};
	};
	
	//		b
	//	a		c
	auto ac_line = create_line_func(v1, v2);
	auto bc_line = create_line_func(v2, v3);
	float alpha = bc_line(point) / bc_line(v1.head<2>());
	float beta = ac_line(point) / ac_line(v2.head<2>());
	return { alpha, beta, (1.f-alpha-beta) };
}


float Draw::radians(float angle) {
	return angle / 180.f * M_PI;
}

mat4 Draw::viewport(int width, int height) {
	return mat4 {
		width/2.f,	0,			0,			width/2.f,
		0,			height/2.f,	0,			height/2.f,
		0,			0,			1,			0,
		0,			0,			0,			1,
	};
}

mat4 Draw::ortho(float fov, float aspect, float n, float f) {
	float t = std::tan(radians(fov / 2.f)) * (f-n);
	float b = -t;
	float r = t * aspect;
	float l = -r;
	mat4 scale = {
		2.f/(r-l),	0,			0,			0,
		0,			2.f/(t-b),	0,			0,
		0,			0,			2.f/(f-n),	0,
		0,			0,			0,			1
	};
	mat4 trans = {
		1,	0,	0,	-2.f/(r-l),
		0,	1,	0,	-2.f/(t-b),
		0,	0,	1,	-2.f/(f-n),
		0,	0,	0,	1
	};
	return scale * trans;
}

mat4 Draw::projection(float fov, float aspect, float n, float f) {
	mat4 perspective = {
		n,		0,		0,		0,
		0,		n,		0,		0,
		0,		0,		n-f,	-n*f,
		0,		0,		1,		0
	};
	return ortho(fov, aspect, n, f) * perspective;
}

mat4 Draw::view(vec3 look_from, vec3 look_up, vec3 look_at) {
	mat4 trans = {
		1, 0, 0, -look_from.x(),
		0, 1, 0, -look_from.y(),
		0, 0, 1, -look_from.z(),
		0, 0, 0, 1
	};

	vec3 z = normalized(look_at - look_from);
	vec3 x = normalized(cross(look_up, z));
	vec3 y = normalized(cross(z, x));
	mat4 rotate = {
		x.x(), y.x(), z.x(), 0,
		x.y(), y.y(), z.y(), 0,
		x.z(), y.z(), z.z(), 0,
		0,	   0,	  0,	 1,
	};
	return transpose(rotate) * trans;
}
