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

void Draw::triangle(FrameBuffer &frame, ShaderBase &shader, std::array<Vertex *, 3> vertice) {
	vec2 bboxmin(std::numeric_limits<float>::max());
	vec2 bboxmax(std::numeric_limits<float>::min());
	for (const Vertex *vertex_ptr : vertice) {
		for (int i = 0; i < 2; ++i) {
			bboxmin[i] = std::min(bboxmin[i], vertex_ptr->position[i]);
			bboxmax[i] = std::max(bboxmax[i], vertex_ptr->position[i]);
		}
	}

	int width = frame.get_width();
	int height = frame.get_height();
	int minx = std::clamp(static_cast<int>(std::floor(bboxmin.x())), 0, width-1);
	int miny = std::clamp(static_cast<int>(std::floor(bboxmin.y())), 0, height-1);
	int maxx = std::clamp(static_cast<int>(std::ceil(bboxmax.x())), 0, width-1);
	int maxy = std::clamp(static_cast<int>(std::ceil(bboxmax.y())), 0, height-1);
	for (int x = minx; x <= maxx; ++x) {
		for (int y = miny; y <= maxy; ++y) {
			const vec3 &v1 = vertice[0]->position.head<3>();
			const vec3 &v2 = vertice[1]->position.head<3>();
			const vec3 &v3 = vertice[2]->position.head<3>();
			float fx = static_cast<float>(x);
			float fy = static_cast<float>(y);
			vec3 coords = barycentric_coord({ fx, fy }, v1, v2, v3);
			if (!(coords[0] >= 0.f) || !(coords[1] >= 0.f) || !(coords[2] >= 0.f))
				continue;
		
			float depth = shader.calc_depth(coords, vertice);
			vec3 point = { fx, fy, depth };
			if (!frame.check_depth(point))
				continue;

			vec3 color;
			if (shader.fragment(point, vertice, color))
				frame.set_color(point, color);
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
			return (y0-y1)*p.x() + (x1-x0)*p.y() + x0*y1 - x1*y0;
		};
	};
	
	//		b
	//	a		c
	auto ac_line = create_line_func(v1, v3);
	auto bc_line = create_line_func(v2, v3);
	float alpha = bc_line(point) / bc_line(v1.head<2>());
	float beta = ac_line(point) / ac_line(v2.head<2>());
	return { alpha, beta, (1.f-alpha-beta) };
}


float Draw::radians(float angle) {
	return angle / 180.f * M_PI;
}


float Draw::random() noexcept {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.f, 1.f);
	return dis(gen);
}

mat4 Draw::ortho(float fov, float aspect, float n, float f) {
	float t = std::tan(radians(fov / 2.f)) * std::abs(n);
	float b = -t;
	float r = t * aspect;
	float l = -r;
	n = -std::abs(n);
	f = -std::abs(f);
	mat4 scale = {
		2.f/(r-l),	0,			0,			0,
		0,			2.f/(t-b),	0,			0,
		0,			0,			2.f/(n-f),	0,
		0,			0,			0,			1
	};
	mat4 trans = {
		1,	0,	0,	-(l+r)/2.f,
		0,	1,	0,	-(b+t)/2.f,
		0,	0,	1,	-(n+f)/2.f,
		0,	0,	0,	1
	};
	auto res = scale * trans;
	return res;
}

mat4 Draw::projection(float fov, float aspect, float n, float f) {
	n = -std::abs(n);
	f = -std::abs(f);
	mat4 perspective = {
		n,		0,		0,		0,
		0,		n,		0,		0,
		0,		0,		n+f,	-n*f,
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

	vec3 z = normalized(look_from - look_at);
	vec3 x = normalized(cross(look_up, z));
	vec3 y = normalized(cross(z, x));
	mat4 rotate = {
		x.x(), y.x(), z.x(), 0,
		x.y(), y.y(), z.y(), 0,
		x.z(), y.z(), z.z(), 0,
		0,	   0,	  0,	 1,
	};
	auto transpose_rotate = transpose(rotate);
	return transpose_rotate * trans;
}

mat4 Draw::rotate_y(float angle) {
	float radian = Draw::radians(angle);
	float cosine = std::cos(radian);
	float sine = std::sin(radian);
	return {
		cosine,	0,		-sine,	0,
		0,		1,		0,		0,
		sine,	0,		cosine,	0,
		0,		0,		0,		1,
	};
}

mat4 Draw::rotate_x(float angle) {
	float radian = Draw::radians(angle);
	float cosine = std::cos(radian);
	float sine = std::sin(radian);
	return {
		1,		0,		0,		0,
		0,		cosine, -sine,	0,
		0,		sine,	cosine, 0,
		0,		0,		0,		1,
	};
}

int Draw::plane_cutting(std::vector<Vertex> &vertices, std::vector<int> &indices) {
	static std::tuple<int, bool(*)(float, float), bool>  planes[] = {
		{ 3,	outside_w_plane,		true  },		// w		butting
		//{ 1,	outside_top_plane,		true  },		// top		cutting
		//{ 1,	outside_bottom_plane,	false },		// buttom	butting
		//{ 0,	outside_right_plane,	true  },		// right	cutting
		//{ 0,	outside_left_plane,		false },		// left		cutting
		//{ 2,	outside_front_plane,	true  },		// front	butting
		//{ 2,	outside_back_plane,		false },		// back		butting
	};

	int size = static_cast<int>(indices.size());
	for (auto iter = std::begin(planes); iter != std::end(planes) && size != 0; ++iter) {
		assert(indices.size() % 3 == 0);
		int limit = static_cast<int>(indices.size()) - 2;
		for (int i = 0; i < limit && size != 0; i += 3) {
			if (indices[i] < 0)
				continue;

			std::span<int, 3> triangle_view(&indices[i], 3);
			auto&&[plane_idx, callback, symbol] = *iter;
			auto res = plane_cutting_triangle(vertices, indices, triangle_view, plane_idx, callback, symbol);
			size += res;
		}
	}
	return size;
}
int Draw::plane_cutting_triangle(std::vector<Vertex> &vertices, std::vector<int> &indices, 

								 std::span<int, 3> triangle_view, size_t plane_idx,
								 bool (*outside_func)(float, float), bool symobl) 
{
	std::array<bool, 3> flags = { false };
	std::array<int, 3> outside_list = { -1 };
	int outside_size = 0;
	for (int i = 0; int idx : triangle_view) {
		const Vertex &v = vertices[idx];
		bool outside = outside_func(v.position[plane_idx], v.position.w());
		if (outside) {
			flags[i] = true;
			outside_list[outside_size] = idx;
			++outside_size;
		}
		++i;
	}
	
	if (outside_size == 0) {
		return 0;
	} else if (outside_size == 1) {
		auto iter = std::find(flags.begin(), flags.end(), true);
		assert(iter != flags.end());
		int outside_idx = static_cast<int>(iter - flags.begin());
		int inside_idx1 = (outside_idx + 2) % 3;
		int inside_idx2 = (outside_idx + 1) % 3;
		int outside_ver_idx = triangle_view[outside_idx];
		int inside_ver_idx1 = triangle_view[inside_idx1];
		int inside_ver_idx2 = triangle_view[inside_idx2];
		const Vertex &outside_vertex = vertices[outside_ver_idx];
		const Vertex &inside_vertex1 = vertices[inside_ver_idx1];
		const Vertex &inside_vertex2 = vertices[inside_ver_idx2];
		const Vertex P = interp_vertex(inside_vertex1, outside_vertex, plane_idx, symobl);
		const Vertex N = interp_vertex(inside_vertex2, outside_vertex, plane_idx, symobl);
		vertices[outside_ver_idx] = P;		// 第一个三角形

		// 第二个三角形
		vertices.push_back(N);
		indices.push_back(inside_ver_idx2);
		indices.push_back(outside_ver_idx);
		indices.push_back(static_cast<int>(vertices.size())-1);
		return 3;

	} else if (outside_size == 2) {
		auto iter = std::find(flags.begin(), flags.end(), false);
		assert(iter != flags.end());
		int inside_idx = static_cast<int>(iter - flags.begin());
		int outside_ver_idx1 = outside_list[0];
		int outside_ver_idx2 = outside_list[1];
		const Vertex &inside_vertex = vertices[triangle_view[inside_idx]];
		const Vertex &outside_vertex1 = vertices[outside_ver_idx1];
		const Vertex &outside_vertex2 = vertices[outside_ver_idx2];
		vertices[outside_ver_idx1] = interp_vertex(inside_vertex, outside_vertex1, plane_idx, symobl);
		vertices[outside_ver_idx2] = interp_vertex(inside_vertex, outside_vertex2, plane_idx, symobl);
		return 0;
	} else if (outside_size == 3) {
		std::fill(triangle_view.begin(), triangle_view.end(), -1);
		return -3;
	}
	assert(false);
	return -1;
}

Vertex Draw::interp_vertex(const Vertex &start, const Vertex &last, size_t plane_idx, bool symbol) {
	float t;
	const vec4 &start_position = start.position;
	const vec4 &last_position = last.position;
	if (plane_idx == 3) {
		t = (plane_w_limit - start_position.w()) / (last_position.w() - start_position.w());
	} else {
		if (symbol) {
			float t1 = start_position.w() - start_position[plane_idx];
			float t2 = last_position.w() - last_position[plane_idx];
			t = t1 / (t1 - t2);
		} else {
			float t1 = start_position.w() + start_position[plane_idx];
			float t2 = last_position.w() + last_position[plane_idx];
			t = t1 / (t1 - t2);
		}
	}
	auto offset = last - start;
	return start + (offset * t);
}

bool Draw::outside_left_plane(float x, float w) {
	return x > -w;
}

bool Draw::outside_right_plane(float x, float w) {
	return x < w;
}

bool Draw::outside_top_plane(float y, float w) {
	return y < w;
}

bool Draw::outside_bottom_plane(float y, float w) {
	return y > -w;
}

bool Draw::outside_back_plane(float z, float w) {
	return z < w;
}

bool Draw::outside_front_plane(float z, float w) {
	return z > -w;
}

bool Draw::outside_w_plane(float f, float w) {
	return w >= plane_w_limit;
}
