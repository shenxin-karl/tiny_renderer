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

void Draw::triangle(FrameBuffer &frame, ShaderBase &shader, std::array<VertexRes *, 3> vertice) {
	vec2 bboxmin(std::numeric_limits<float>::max());
	vec2 bboxmax(std::numeric_limits<float>::min());
	for (const VertexRes *vertex_ptr : vertice) {
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
			const VertexRes *v1 = vertice[0];
			const VertexRes *v2 = vertice[1];
			const VertexRes *v3 = vertice[2];
			const vec3 &v1_point = v1->position.head<3>();
			const vec3 &v2_point = v2->position.head<3>();
			const vec3 &v3_point = v3->position.head<3>();
			float fx = static_cast<float>(x);
			float fy = static_cast<float>(y);
			vec3 coords = barycentric_coord({ fx, fy }, v1_point, v2_point, v3_point);
			if (!(coords[0] >= 0.f) || !(coords[1] >= 0.f) || !(coords[2] >= 0.f))
				continue;
		
			float depth = shader.calc_depth(coords, vertice);
			vec3 point = { fx, fy, depth };
			if (!frame.check_depth(point))
				continue;

			vec3 color;
			auto args = v1->args->interp(v2->args, v3->args, coords, depth);
			if (shader.fragment(point, args, color))
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

bool Draw::plane_cutting(std::vector<VertexRes> &out_vertices) {
	static std::tuple<bool(*)(float, float), int, float(*)(const vec4 &, const vec4 &)> plane_args[] = {
		{ outside_w_plane,      3, get_w_plane_ratio		   },
		{ outside_left_plane,   0, get_negative_plane_ratio<0> },
		{ outside_right_plane,  0, get_positive_plane_ratio<0> },
		{ outside_bottom_plane, 1, get_negative_plane_ratio<1> },
		{ outside_top_plane,    1, get_positive_plane_ratio<1> },
		{ outside_far_plane,    2, get_negative_plane_ratio<2> },
		{ outside_near_plane,   2, get_positive_plane_ratio<2> },
	};

	for (auto iter = std::begin(plane_args); iter != std::end(plane_args); ++iter) {
		std::vector<VertexRes> input(std::move(out_vertices));
		auto &&[check_func, index, get_radio] = *iter;
		size_t input_size = input.size();
		for (size_t i = 0; i < input_size; ++i) {
			VertexRes &curr = input[i];
			VertexRes &next = input[(i+1) % input_size];
			int curr_outside = check_func(curr.position[index], curr.position.w());
			int next_outside = check_func(next.position[index], next.position.w());
			int flag = curr_outside | (next_outside << 1);
			switch (flag) {
			case 0:				// not outside
				out_vertices.push_back(curr);
				break;
			case 1:				// curr outside
				out_vertices.push_back(interp_vertex_res(next, curr, get_radio(next.position, curr.position)));
				break;
			case 2:				// next outside
				out_vertices.push_back(curr);
				out_vertices.push_back(interp_vertex_res(curr, next, get_radio(curr.position, next.position)));
				break;
			case 3:				// all outside
				break;
			}
		}
	}
	return out_vertices.size() >= 3;
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

bool Draw::outside_near_plane(float z, float w) {
	return z < w;
}

bool Draw::outside_far_plane(float z, float w) {
	return z > -w;
}

bool Draw::outside_w_plane(float, float w) {
	return w >= plane_w_limit;
}

VertexRes Draw::interp_vertex_res(const VertexRes &start, const VertexRes &last, float t) {
	return {
		start.position + (t * (last.position - start.position)),
		start.args->interp(last.args, t),
	};
}