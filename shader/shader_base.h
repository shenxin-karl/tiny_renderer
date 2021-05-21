#pragma once

template<typename T>
concept uniform_key_constraint = requires {
	std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, float> ||
	std::is_same_v<T, vec2> || std::is_same_v<T, vec3> || std::is_same_v<T, vec4> ||
	std::is_same_v<T, mat2> || std::is_same_v<T, mat3> || std::is_same_v<T, mat4>;
};


struct ShaderBase {
	virtual ~ShaderBase() = default;
	virtual vec4 vertex(const Vertex &vertex, int idx) = 0;
	virtual bool fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) = 0;

	template<typename T>
	constexpr T interp(const T &v1, const T &v2, const T &v3) const noexcept {
		T res1 = v1 * coords[0];
		T res2 = v2 * coords[1];
		T res3 = v3 * coords[2];
		return (res1 + res2 + res3) / inverse_z;
	}

	template<typename T>
	constexpr T interp(const std::array<Vertex *, 3> &vertices, T Vertex:: *mem_ptr) const noexcept {
		return interp(vertices[0]->*mem_ptr, vertices[1]->*mem_ptr, vertices[2]->*mem_ptr);
	}

	void set_model(const mat4 &_model);
	void set_view(const mat4 &_view);
	void set_projection(const mat4 &_projection);
	void set_viewport(const mat4 &_viewport);
	float calc_depth(const vec3 &_coords, const std::array<Vertex *, 3> &vertices);
protected:
	void update_mvp();
	vec3 coords;			// 重心坐标
	mat4 view;			
	mat4 projection;	
	mat4 model;		
	mat4 viewport;
	mat4 mvp;
	float inverse_z = 1.f;	// z 倒数
private:
	using uniform_key_type = std::string;
	using uniform_value_type = std::variant<bool, int, float, vec2, vec3, vec4, mat2, mat3, mat4, Texture2d>;
	std::unordered_map<uniform_key_type, uniform_value_type> uniforms;
public:
	template<uniform_key_constraint T>
	const T &get_uniform(const uniform_key_type &key) const {
		auto iter = uniforms.find(key);
		assert(iter != uniforms.end());
		return std::get<T>(iter->second);
	}

	template<uniform_key_constraint T>
	void set_uniform(const uniform_key_type &key, const T &value) {
		uniforms.emplace(key, uniform_value_type(value));
	}
};