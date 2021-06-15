#include "common.h"

ParallaxMappingShaderArgs::ParallaxMappingShaderArgs(const vec2 &texcoord, const vec3 &tangent_position, 
	const vec3 &tangent_eye_pos, const vec3 &tangent_light_dir) 
: our_texcoord(texcoord), our_tangent_position(tangent_position)
, our_tangent_eye_pos(tangent_eye_pos), our_tangent_light_dir(tangent_light_dir) {

}

SArgsPtr ParallaxMappingShaderArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const auto *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &ParallaxMappingShaderArgs::our_texcoord, t),
		common_interp(this, rhs, &ParallaxMappingShaderArgs::our_tangent_position, t),
		common_interp(this, rhs, &ParallaxMappingShaderArgs::our_tangent_eye_pos, t),
		common_interp(this, rhs, &ParallaxMappingShaderArgs::our_tangent_light_dir, t)
	);
}

SArgsPtr ParallaxMappingShaderArgs::interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept {
	const auto *v1_ptr = this;
	const auto *v2_ptr = SArgsPtr_cast(v2);
	const auto *v3_ptr = SArgsPtr_cast(v3);
	return Make_ArgsPtr(
		common_interp(v1_ptr, v2_ptr, v3_ptr, &ParallaxMappingShaderArgs::our_texcoord, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &ParallaxMappingShaderArgs::our_tangent_position, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &ParallaxMappingShaderArgs::our_tangent_eye_pos, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &ParallaxMappingShaderArgs::our_tangent_light_dir, coord, depth)
	);
}

void ParallaxMappingShaderArgs::perspective_divide(float inverse_z) noexcept {
	our_texcoord *= inverse_z;
	our_tangent_position *= inverse_z;
	our_tangent_eye_pos *= inverse_z;
	our_tangent_light_dir *= inverse_z;
}

void ParallaxMappingShader::initialize() noexcept {
	uniform_eye_pos = get_uniform<vec3>("eye_pos");
	uniform_light_dir = get_uniform<vec3>("light_dir");
	uniform_normal_matrix = transpose(inverse(mat3(model)));
	uniform_diffuse_map = get_uniform<Texture2d>("diffuse_map");
	uniform_normal_map = get_uniform<Texture2d>("normal_map");
	uniform_depth_map = get_uniform<Texture2d>("depth_map");
	uniform_height_scale = get_uniform<float>("height_scale");
	uniform_light_ambient = get_uniform<vec3>("light_ambient");
	uniform_light_diffuse = get_uniform<vec3>("light_diffuse");
	uniform_light_specular = get_uniform<vec3>("light_specular");
	uniform_specular_factor = get_uniform<float>("specular_factor");
}

vec4 ParallaxMappingShader::vertex(const Vertex &vertex, SArgsPtr &args) noexcept {
	vec3 T = normalized(uniform_normal_matrix * vertex.tangent);
	vec3 B = normalized(uniform_normal_matrix * vertex.bitangent);
	vec3 N = normalized(uniform_normal_matrix * vertex.normal);
	mat3 reverse_tbn = inverse_tbn(T, B, N);
	vec3 model_position = vec3(model * vertex.position);
	args = std::make_shared<ParallaxMappingShaderArgs>(
		vertex.texcoords,
		reverse_tbn * model_position,
		reverse_tbn * uniform_eye_pos,
		reverse_tbn * uniform_light_dir
	);
	return mvp * vertex.position;
}

bool ParallaxMappingShader::fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept {
	const auto *args_ptr = static_cast<const ParallaxMappingShaderArgs *>(args.get());

	vec3 view_dir = normalized(args_ptr->our_tangent_eye_pos - args_ptr->our_tangent_position);
	//vec2 depth_texcoord = parallax_mapping_texcoord(view_dir, args_ptr->our_texcoord);
	//vec2 depth_texcoord = steep_parallax_mapping_texcoord(view_dir, args_ptr->our_texcoord);
	vec2 depth_texcoord = parallax_occlusion_mapping(view_dir, args_ptr->our_texcoord);
	if (depth_texcoord.s() < 0.f || depth_texcoord.s() > 1.f || depth_texcoord.t() < 0.f || depth_texcoord.t() > 1.f)
		return false;

	vec3 normal = uniform_normal_map.normal(depth_texcoord);
	vec3 diffuse_color = uniform_diffuse_map.rgb(depth_texcoord);

	// ambient
	vec3 ambinet = diffuse_color * uniform_light_ambient;
	
	// diffuse
	vec3 light_dir = normalized(args_ptr->our_tangent_light_dir);
	float diff = std::max(dot(light_dir, normal), 0.f);
	vec3 diffuse = diff * uniform_light_diffuse * diffuse_color;

	// specular
	vec3 half_vec = normalized(view_dir + light_dir);
	float spec = std::pow(std::max(dot(half_vec, normal), 0.f), uniform_specular_factor);
	vec3 specular = spec * diffuse_color * uniform_light_specular;

	color = ambinet + diffuse + specular;
	return true;
}

vec2 ParallaxMappingShader::parallax_mapping_texcoord(const vec3 &view_dir, const vec2 &texcoord) const {
	float depth = uniform_depth_map.rgb(texcoord).r();
	vec2 p = (vec2(view_dir) / view_dir.z()) * (depth * uniform_height_scale);
	return texcoord - p;
}

vec2 ParallaxMappingShader::steep_parallax_mapping_texcoord(const vec3 &view_dir, vec2 texcoord) const {
	float depth = uniform_depth_map.rgb(texcoord).r();
	vec2 p = (vec2(view_dir)) / view_dir.z() * (depth * uniform_height_scale);
	constexpr float min_layers = 4.f;
	constexpr float max_layers = 16.f;
	float layers = Draw::mix(max_layers, min_layers, std::abs(view_dir.z()));

	vec2 delta_p = p / layers;
	float delta_depth = 1.f / layers;
	float curr_depth = 1.f;
	float expect_depth = 0.f;
	while (curr_depth > expect_depth) {
		expect_depth += delta_depth;
		texcoord -= delta_p;
		curr_depth = uniform_depth_map.rgb(texcoord).r();
	}
	return texcoord;
}

vec2 ParallaxMappingShader::parallax_occlusion_mapping(const vec3 &view_dir, vec2 texcoord) const {
	float depth = uniform_depth_map.rgb(texcoord).r();
	vec2 p = (vec2(view_dir)) / view_dir.z() * (depth * uniform_height_scale);
	constexpr float min_layers = 4.f;
	constexpr float max_layers = 16.f;
	float layers = Draw::mix(max_layers, min_layers, std::abs(view_dir.z()));

	vec2 delta_p = p / layers;
	float delta_depth = 1.f / layers;
	float curr_depth = 1.f;
	float expect_depth = 0.f;
	while (curr_depth > expect_depth) {
		expect_depth += delta_depth;
		texcoord -= delta_p;
		curr_depth = uniform_depth_map.rgb(texcoord).r();
	}

	vec2 prev_texcoord = texcoord + delta_p;
	float prev_depth = uniform_depth_map.rgb(prev_texcoord).r();
	float after_depth = curr_depth - expect_depth;
	float before_depth = prev_depth - expect_depth + delta_depth;
	float weight = after_depth / (after_depth - before_depth);
	return Draw::mix(texcoord, prev_texcoord, weight);
}
