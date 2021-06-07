#pragma once

struct ShaderArgsBase;
using SArgsPtr = std::shared_ptr<ShaderArgsBase>;
struct ShaderArgsBase {
	virtual ~ShaderArgsBase() = default;
	virtual SArgsPtr interp(const SArgsPtr &other, float t) const noexcept;
	virtual SArgsPtr interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept;
	virtual void perspective_divide(float inverse_z) noexcept;
};

template<typename T>
using _rm_const_point_t = std::remove_const_t<std::remove_pointer_t<T>>;

template<typename T>
constexpr std::remove_const_t<T> _sargs_ptr_cast(SArgsPtr &other) noexcept {
	return static_cast<T>(other.get());
}

template<typename T>
constexpr std::add_const_t<T> _sargs_ptr_cast(const SArgsPtr &other) noexcept {
	return static_cast<std::add_const_t<T>>(other.get());
}

struct LightShaderArgs;
template<typename T, typename... Args>
constexpr SArgsPtr _make_sargs_ptr(Args&&... args) {
	using type = _rm_const_point_t<T>;
	return std::make_shared<type>(std::forward<Args>(args)...);
}

template<typename T1, typename T2>
constexpr T2 common_interp(const T1 &v1, const T1 &v2, T2 (_rm_const_point_t<T1>::*mem_ptr), float t) {
	return v1->*mem_ptr + ((v2->*mem_ptr - v1->*mem_ptr) * t);
}

template<typename T1, typename T2>
constexpr T2 common_interp(const T1 &v1, const T1 &v2, const T1 &v3, T2 (_rm_const_point_t<T1>::*mem_ptr),
						   const vec3 &coord, float depth) 
{
	auto t1 = v1->*mem_ptr * coord[0];
	auto t2 = v2->*mem_ptr * coord[1];
	auto t3 = v3->*mem_ptr * coord[2];
	return (t1 + t2 + t3) * depth;
}

#define SArgsPtr_cast(ptr) _sargs_ptr_cast<decltype(this)>(ptr)
#define Make_ArgsPtr(...) _make_sargs_ptr<decltype(this)>(__VA_ARGS__)