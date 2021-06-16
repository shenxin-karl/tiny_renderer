#pragma once

class TextureCube;
template<typename T>
concept uniform_key_constraint = (
	std::same_as<T, bool> || std::same_as<T, int> || std::same_as<T, float> ||
	std::same_as<T, vec2> || std::same_as<T, vec3> || std::same_as<T, vec4> ||
	std::same_as<T, mat2> || std::same_as<T, mat3> || std::same_as<T, mat4> ||
	std::same_as<T, Texture2d> || std::same_as<T, TextureCube>
);

struct VertexRes {
	vec4		position;
	SArgsPtr	args;
};

struct ShaderBase {
	virtual ~ShaderBase() = default;
	virtual void initialize() noexcept {};			// It is called once per frame
	virtual vec4 vertex(const Vertex &vertex, SArgsPtr &args) noexcept = 0;
	virtual bool fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept = 0;

	void set_model(const mat4 &_model);
	void set_view(const mat4 &_view);
	void set_projection(const mat4 &_projection);

	// face_func accepts a value for the cosine of the sum normal, and returns true to eliminate triangles
	void set_face_culling_func(const std::function<bool(float)> &face_func);
	bool is_enable_face_culling() const noexcept;
	const std::function<bool(float)> &get_face_culling_func() const noexcept;
protected:
	void update_mvp();
	mat4 view;			
	mat4 projection;	
	mat4 model;		
	mat4 viewport;
	mat4 mvp;
	std::function<bool(float)> face_culling_func;		// ±³ÃæÌÞ³ýº¯Êý
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