#pragma once

class TextureCube;

template<typename T>
concept uniform_key_constraint = requires {
	std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, float> ||
	std::is_same_v<T, vec2> || std::is_same_v<T, vec3> || std::is_same_v<T, vec4> ||
	std::is_same_v<T, mat2> || std::is_same_v<T, mat3> || std::is_same_v<T, mat4>;
};


struct ShaderBase {
	virtual ~ShaderBase() = default;
	virtual void initialize() noexcept {};			// It is called once per frame
	virtual vec4 vertex(const Vertex &vertex, int idx) noexcept = 0;
	virtual bool fragment(const vec3 &point, vec3 &color) noexcept = 0;

	template<typename T>
	constexpr T interp(const T &v1, const T &v2, const T &v3) const noexcept {
		T res1 = v1 * coords[0];
		T res2 = v2 * coords[1];
		T res3 = v3 * coords[2];
		return (res1 + res2 + res3) * depth;
	}

	template<typename T>
	constexpr T interp(const std::array<T, 3> &vertices) const noexcept {
		return interp(vertices[0], vertices[1], vertices[2]);
	}

	void set_model(const mat4 &_model);
	void set_view(const mat4 &_view);
	void set_projection(const mat4 &_projection);
	float calc_depth(const vec3 &_coords, const std::array<Vertex *, 3> &vertices);
	float get_depth() const noexcept;

	// face_func accepts a value for the cosine of the sum normal, and returns true to eliminate triangles
	void set_face_culling_func(const std::function<bool(float)> &face_func);
	bool is_enable_face_culling() const noexcept;
	const std::function<bool(float)> &get_face_culling_func() const noexcept;
protected:
	void update_mvp();
	vec3 coords;			// 重心坐标
	mat4 view;			
	mat4 projection;	
	mat4 model;		
	mat4 viewport;
	mat4 mvp;
	float depth;
	std::function<bool(float)> face_culling_func;		// 背面剔除函数
private:
	using uniform_key_type = std::string;
	using uniform_value_type = std::variant<bool, int, float, vec2, vec3, vec4, mat2, mat3, mat4, Texture2d, TextureCube>;
	std::unordered_map<uniform_key_type, uniform_value_type> uniforms;
public:
	template<uniform_key_constraint T>
	const T &get_uniform(const uniform_key_type &key) const noexcept {
		auto iter = uniforms.find(key);
		assert(iter != uniforms.end());
		return std::get<T>(iter->second);
	}

	template<uniform_key_constraint T>
	void set_uniform(const uniform_key_type &key, const T &value) noexcept {
		uniforms[key] = uniform_value_type(value);
	}
};