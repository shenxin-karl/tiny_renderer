#pragma once

#ifdef _MSC_VER
#define MSVC_EBCO __declspec(empty_bases) 
#else
#define NSVC_ECBO
#endif // _MSC_VER



template<typename T, size_t I>
using identity_t = T;

template<typename T, size_t N, typename Seq = std::make_index_sequence<N>, bool = (N > 1)>
struct MSVC_EBCO vec;

template<typename T, size_t N, size_t NV, typename Seq1, typename Seq2, bool = (NV > 0 && NV != N)>
struct MSVC_EBCO VecBaseImpl {
    // empty
};

template<typename T, size_t N, size_t NV, size_t...I1, size_t...I2>
struct MSVC_EBCO VecBaseImpl<T, N, NV, std::index_sequence<I1...>, std::index_sequence<I2...>, true> {
public:
    constexpr VecBaseImpl() = default;
    constexpr VecBaseImpl(const vec<T, NV> &v, identity_t<T, I2>... scalar) noexcept {
        new (static_cast<vec<T, N> *>(this)->data) T[N]{ v[I1]..., decltype(I2, T{})(scalar)... };
    }
    constexpr VecBaseImpl(identity_t<T, I2>... scalar, const vec<T, NV> &v) noexcept {
        new (static_cast<vec<T, N> *>(this)->data) T[N] { scalar..., v[I1]... };
    }
};

template<typename T, size_t N, typename Seq>
struct MSVC_EBCO VecBase;

template<typename T, size_t N, size_t ...I>
struct MSVC_EBCO VecBase<T, N, std::index_sequence<I...>> 
    : public VecBaseImpl<T, N, I, std::make_index_sequence<I>, std::make_index_sequence<N-I>>... {

    using VecBaseImpl<T, N, I, std::make_index_sequence<I>, std::make_index_sequence<N-I>>::VecBaseImpl...;
};


template<typename T, size_t N, size_t ...I>
struct MSVC_EBCO vec<T, N, std::index_sequence<I...>, true> : public VecBase<T, N, std::make_index_sequence<N>> {
    T data[N];
public:
    constexpr explicit vec(T scalar = T{}) noexcept : data{ identity_t<T, I>(scalar)... } {}
    constexpr vec(identity_t<T, I>... scalars) noexcept : data{ scalars... } {}
    constexpr vec(const vec &) = default;
    constexpr vec(vec &&) noexcept = default;
    constexpr vec &operator=(const vec &) = default;

    template<size_t UN, typename USeq> requires (UN > N)
    explicit constexpr vec(const vec<T, UN, USeq> &other) noexcept {
        *this = other;
    }

    using VecBase<T, N, std::make_index_sequence<N>>::VecBase;

    constexpr const T &operator[](size_t i) const noexcept {
        assert(i < N);
        return data[i];
    }

    constexpr T &operator[](size_t i) {
		assert(i < N);
		return data[i];
    }

    template<size_t UN> requires (UN <= N)
    constexpr const vec<T, UN> &head() const noexcept {
		return reinterpret_cast<const vec<T, UN> &>(*this);
    }

    template<size_t UN> requires (UN <= N)
    constexpr vec<T, UN> &head() noexcept {
		return reinterpret_cast<vec<T, UN> &>(*this);
    }

    template<size_t UN, typename USeq> requires (UN >= N)
    vec &operator=(const vec<T, UN, USeq> &other) noexcept {
        std::memcpy(data, other.data, sizeof(data));
        return *this;
    }

    T dot(const vec &other) const noexcept {
        T sum = T{};
        for (int i = 0; i < N; ++i) 
            sum += (data[i] * other.data[i]);
        return sum;
    }

    T length2() const noexcept {
        return dot(*this);
    }

    T length() const noexcept {
        auto len2 = length2();
        if (len2 != 0)
            return std::sqrt(len2);
        else
            return T{};
    }

    void normalize() noexcept {
        if (auto len2 = length2(); len2 != 0) {
            float inverse_len = static_cast<T>(1) / std::sqrt(len2);
            *this *= inverse_len;
        }
    }

    vec normalized() const noexcept {
        T len2 = length2();
        auto res = *this;
        if (len2 != 0) {
            T invserse_len = static_cast<T>(1) / std::sqrt(len2);
            res *= invserse_len;
            return res;
        } else {
            return res;
        }
    }

    vec cross(const vec &other) const noexcept {
        static_assert(N == 3, "cross only vec3 use");
        return {
            y() * other.z() - z() * other.y(),
            z() * other.x() - x() * other.z(),
            x() * other.y() - y() * other.x(),
        };
    }

    constexpr size_t size() const noexcept {
        return N;
    }

#define VEC_GET_FUN(f, n)                   \
    constexpr T &f() noexcept {             \
        static_assert(n < N);               \
        return this->data[n];               \
    }                                       \
    constexpr const T &f() const noexcept { \
        static_assert(n < N);               \
        return this->data[n];               \
    }
    VEC_GET_FUN(x, 0)
    VEC_GET_FUN(y, 1)
    VEC_GET_FUN(z, 2)
    VEC_GET_FUN(w, 3)
    VEC_GET_FUN(r, 0)
    VEC_GET_FUN(g, 1)
    VEC_GET_FUN(b, 2)
    VEC_GET_FUN(a, 3)
    VEC_GET_FUN(s, 0)
    VEC_GET_FUN(t, 1)
#undef VEC_GET_FUN

    constexpr auto begin() noexcept { return std::begin(this->data); }
    constexpr auto begin() const noexcept { return std::begin(this->data); }
    constexpr auto end() noexcept { return std::end(this->data); }
    constexpr auto end() const noexcept { return std::end(this->data); }
    constexpr T *value_ptr() const noexcept {
        return data;
    }

    static const vec &identity() noexcept {
        constexpr static vec zero_vec{ T{} };
        return zero_vec;
    }

    friend vec &operator +=(vec &lhs, const vec &rhs) noexcept {             
        return lhs.transfrom<std::plus<T>>(rhs);                                        
    }                                                                               
    friend vec operator+(const vec &lhs, const vec &rhs) noexcept {         
        auto res = lhs;                                                             
        res += rhs;                                                                
        return res;                                                                 
    }                                                                               
    friend vec &operator+=(vec &vec, const T &f) noexcept {                 
        return vec.transfrom<std::plus<T>>(f);                                          
    }                                                                               
    friend vec operator+(const vec &vec, const T &f) noexcept {             
        auto res = vec;                                                             
        res += f;                                                                  
        return res;                                                                 
    }
    
    friend vec &operator-=(vec &lhs, const vec &rhs) noexcept {             
        return lhs.transfrom<std::minus<T>>(rhs);                                        
    }                                                                               
    friend vec operator-(const vec &lhs, const vec &rhs) noexcept {         
        auto res = lhs;                                                             
        res -= rhs;                                                                
        return res;                                                                 
    }                                                                               
    friend vec &operator-=(vec &vec, const T &f) noexcept {                 
        return vec.transfrom<std::minus<T>>(f);                                          
    }                                                                               
    friend vec operator-(const vec &vec, const T &f) noexcept {             
        auto res = vec;                                                             
        res -= f;                                                                  
        return res;                                                                 
    }

    friend vec &operator*=(vec &lhs, const vec &rhs) noexcept {             
        return lhs.transfrom<std::multiplies<T>>(rhs);                                        
    }                                                                               
    friend vec operator*(const vec &lhs, const vec &rhs) noexcept {         
        auto res = lhs;                                                             
        res *= rhs;                                                                
        return res;                                                                 
    }                                                                               
    friend vec &operator*=(vec &vec, const T &f) noexcept {                 
        return vec.transfrom<std::multiplies<T>>(f);                                          
    }                                                                               
    friend vec operator*(const vec &vec, const T &f) noexcept {             
        auto res = vec;                                                             
        res *= f;                                                                  
        return res;                                                                 
    }
    friend vec &operator*=(const T &f, vec &vec) noexcept {
        for (auto &v : vec)
            v *= f;
        return vec;
    }
    friend vec operator*(const T &f, const vec &vec) noexcept {
        auto res = vec;
        res *= f;
        return res;
    }

    friend vec &operator/=(vec &lhs, const vec &rhs) noexcept {             
        return lhs.transfrom<std::divides<T>>(rhs);                                        
    }                                                                               
    friend vec operator/(const vec &lhs, const vec &rhs) noexcept {         
        auto res = lhs;                                                             
        res /= rhs;                                                                
        return res;                                                                 
    }                                                                               
    friend vec &operator/=(vec &vec, const T &f) noexcept {   
        T inverse_f = static_cast<T>(1) / f;
        return vec.transfrom<std::multiplies<T>>(inverse_f);                                          
    }                                                                               
    friend vec operator/(const vec &vec, const T &f) noexcept {             
        auto res = vec;                                                             
        res /= f;                                                                  
        return res;                                                                 
    }

    vec operator-() const noexcept {
        auto res = *this;
        for (int i = 0; i < N; ++i) 
            res.data[i] = -res.data[i];
        return res;
    }

private:
    template<typename OPF>
    vec &transfrom(const vec &other) noexcept {
        for (int i = 0; i < N; ++i)
            data[i] = OPF()(data[i], other.data[i]);
        return *this;
    }

    template<typename OPF>
    vec &transfrom(const T &f) noexcept {
        for (int i = 0; i < N; ++i)
            data[i] = OPF()(data[i], f);
        return *this;
    }
};

template<typename T, size_t N, typename Seq>
inline T length(const vec<T, N, Seq> &v) noexcept {
    return v.length();
}

template<typename T, size_t N, typename Seq>
inline vec<T, N> normalized(const vec<T, N, Seq> &v) noexcept {
    return v.normalized();
} 

template<typename T>
inline vec<T, 3> cross(const vec<T, 3> &lhs, const vec<T, 3> &rhs) noexcept {
    return lhs.cross(rhs);
}

template<typename T, size_t N, typename Seq>
inline T dot(const vec<T, N, Seq> &lhs, const vec<T, N, Seq> &rhs) {
    return lhs.dot(rhs);
}

using vec2 = vec<float, 2>;
using vec3 = vec<float, 3>;
using vec4 = vec<float, 4>;

static_assert(sizeof(vec2) == (sizeof(float) * 2), "vec2 has no empty accumulation optimization");
static_assert(sizeof(vec3) == (sizeof(float) * 3), "vec3 has no empty accumulation optimization");
static_assert(sizeof(vec4) == (sizeof(float) * 4), "vec4 has no empty accumulation optimization");


template<typename T, size_t N, typename Seq>
inline std::ostream &operator<<(std::ostream &os, const vec<T, N, Seq> &v) {
    for (const auto &f : v) 
        os << f << " ";
    return os;
}

template<typename T, size_t N, typename Seq>
inline std::istream &operator>>(std::istream &is, vec<T, N, Seq> &v) {
    for (auto &f : v)
        is >> f;
    return is;
}

