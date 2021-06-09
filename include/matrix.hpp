#pragma once

template<typename T, size_t N, typename = std::make_index_sequence<N*N>>
struct Matrix;

using mat3 = Matrix<float, 3>;
using mat4 = Matrix<float, 4>;
using mat2 = Matrix<float, 2>;

template<typename T, size_t N, typename Seq>
Matrix<T, N, Seq> transpose(const Matrix<T, N, Seq> &mat);

namespace matrix {
    template<typename T, size_t N, typename = std::make_index_sequence<N*N>> requires (N >=2 && N <= 3)
    struct MatrixDet;
}

template<typename T, size_t N, typename TSeq> requires (N >= 2 && N <= 3)
constexpr Matrix<T, N, TSeq> inverse(const Matrix<T, N, TSeq> &mat);

template<typename T, size_t N, size_t...I>
struct Matrix<T, N, std::index_sequence<I...>> {
    T data[N][N];
public:
    constexpr Matrix() noexcept : Matrix(static_cast<T>((I / N) == (I % N))...) {}
    constexpr Matrix(identity_t<T, I>... scalars) noexcept : data{ scalars... } {}
    constexpr Matrix(const Matrix &) noexcept = default;
    constexpr Matrix &operator=(const Matrix &) noexcept = default;

    template<size_t UN, typename TSeq> requires (UN > N)
    constexpr Matrix(const Matrix<T, UN, TSeq> &mat) {
        for (size_t i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j)
                data[i][j] = mat.data[i][j];
        }
    }

    // 默认对象
    static const Matrix &identity() noexcept {
        constexpr static Matrix obj = { static_cast<T>(I / N == I % N)... };
        return obj;
    }

    constexpr T &operator()(size_t row, size_t col) noexcept {
        return data[row][col];
    }

    constexpr const T &operator()(size_t row, size_t col) const noexcept {
        return data[row][col];
    }

    friend constexpr Matrix transpose(const Matrix &mat) noexcept {
        auto res = mat;
        for (int i = 1; i < N; ++i) {
            for (int j = i-1; j >= 0; --j)
                std::swap(res.data[i][j], res.data[j][i]);
        }
        return res;
    }

    constexpr Matrix &operator*=(const T &f) noexcept {
        (((*this)[I] *= f), ...);
        return *this;
    }

    friend constexpr Matrix operator*(const Matrix &mat, const T &f) noexcept {
        return { (mat[I] * f)... };
    }

    friend constexpr Matrix operator*(const T &f, const Matrix &mat) noexcept {
        return { (mat[I] * f)... };
    }

    constexpr Matrix operator/=(const T &f) noexcept {
        float inverse_f = 1.f / f;
        (((*this)[I] *= inverse_f), ...);
        return *this;
    }

    friend constexpr Matrix operator/(const Matrix &mat, const T &f) noexcept {
        return { (mat[I] / f)... };
    }

    friend constexpr Matrix operator/(const T &f, const Matrix &mat) noexcept {
		return { (mat[I] / f)... };
    }
    
    friend constexpr Matrix operator+(const Matrix &lhs, const Matrix &rhs) noexcept {
        return { (lhs[I] + rhs[I])... };
    }

    friend constexpr Matrix operator-(const Matrix &lhs, const Matrix &rhs) noexcept {
        return { (lhs[I] - rhs[I])... };
    }

    friend constexpr Matrix operator*(const Matrix &lhs, const Matrix &rhs) noexcept {
        auto calc_value = [&](size_t i) {
            size_t row = i / N;
            size_t col = i % N;
            auto sum = T{};
            for (size_t i = 0; i < N; ++i)
                sum += (lhs.data[row][i] * rhs.data[i][col]);
            return sum;
        };

        return { calc_value(I)... };
    }

    friend vec<T, N> operator*(const Matrix &mat, const vec<T, N> &v) noexcept {
        vec<T, N> res;
        for (int row = 0; row < N; ++row) {
            auto sum = T{};
            for (int col = 0; col < N; ++col)
                sum += (mat.data[row][col] * v[col]);
           	res[row] = sum;
        }
        return res;
    }
    
private:
	template<typename T, size_t N, typename TSeq> requires (N >= 2 && N <= 3)
	friend constexpr Matrix<T, N, TSeq> inverse(const Matrix<T, N, TSeq> &mat);

	template<typename, size_t TN, typename> requires (TN >=2 && TN <= 3)
	friend struct matrix::MatrixDet;

    constexpr T &operator[](size_t idx) noexcept {
        return (&data[0][0])[idx];
    }

    constexpr const T &operator[](size_t idx) const noexcept {
		return (&data[0][0])[idx];
    }
};

template<typename T, size_t N, size_t...I>
inline std::ostream &operator<<(std::ostream &os, const Matrix<T, N, std::index_sequence<I...>> &mat) noexcept {
    for (const auto &row : mat.data) {
        for (const T &f : row) 
            os << f << " ";
        os << std::endl;
    }
    return os;
}



namespace matrix {

template<typename T, size_t N, size_t...I> 
struct MatrixDet<T, N, std::index_sequence<I...>> {
    using const_reference = const T &;
    using const_pointer = const T *;
    const_pointer data[N][N];
public:
    MatrixDet(const Matrix<T, N, std::index_sequence<I...>> &mat) : data{ (&mat[I])... } {}
    MatrixDet(identity_t<const_reference, I>... scalars) : data{ (&scalars)... } {}

    
    T value() const noexcept {
        if constexpr (N == 2) {
            // a b
            // c d
            // a*d - b*c
            return get(0, 0) * get(1, 1) - get(0, 1) * get(1, 0);
        } else if constexpr (N == 3) {
            // a b c
            // d e f
            // g h i
            // aei + bfg + cdh - afh - bdi - ceg
            // a(ei - fh) + b(fg - di) + c(dh - eg)
            const T &a = get(0, 0), &b = get(0, 1), &c = get(0, 2);
            const T &d = get(1, 0), &e = get(1, 1), &f = get(1, 2);
            const T &g = get(2, 0), &h = get(2, 1), &i = get(2, 2);
            return (a * (e*i - f*h)) + (b * (f*g - d*i)) + (c * (d*h - e*g));
        } else {
            static_assert(false);
            return T{};
        }
    }
private:
    constexpr const_reference get(int row, int col) const noexcept {
        return *data[row][col];
    }
};

//template<typename T, typename TSeq>
//constexpr Matrix<T, 4, TSeq> adj_matrix(const Matrix<T, 4, TSeq> &mat) {
//	auto &&[a, b, c, d] = mat.data[0];
//	auto &&[e, f, g, h] = mat.data[1];
//	auto &&[i, j, k, l] = mat.data[2];
//	auto &&[m, n, o, p] = mat.data[3];
//
//    using MatrixDelType = MatrixDet<T, 3>;
//    Matrix<T, 4, TSeq> res;
//    res(0, 0) = MatrixDelType(f, g, h, j, k, l, n, o, p).value();
//    res(0, 1) = MatrixDelType(e, g, h, i, k, l, m, o, p).value();
//    res(0, 2) = MatrixDelType(e, f, h, i, j, l, m, n, p).value();
//    res(0, 3) = MatrixDelType(e, f, g, i, j, k, m, n, o).value();
//    res(1, 0) = MatrixDelType(b, c, d, j, k, l, n, o, p).value();
//    res(1, 1) = MatrixDelType(a, c, d, i, k, l, m, o, p).value();
//    res(1, 2) = MatrixDelType(a, b, d, i, j, l, m, n, p).value();
//    res(1, 3) = MatrixDelType(a, b, c, i, j, k, m, n, o).value();
//    res(2, 0) = MatrixDelType(b, c, d, f, g, h, n, o, p).value();
//    res(2, 1) = MatrixDelType(a, c, d, e, g, h, m, o, p).value();
//    res(2, 2) = MatrixDelType(a, b, d, e, f, h, m, n, p).value();
//    res(2, 3) = MatrixDelType(a, b, c, e, f, g, m, n, o).value();
//    res(3, 0) = MatrixDelType(b, c, d, f, g, h, j, k, l).value();
//    res(3, 1) = MatrixDelType(a, c, d, e, g, h, i, k, l).value();
//    res(3, 2) = MatrixDelType(a, b, d, e, f, h, i, j, l).value();
//    res(3, 3) = MatrixDelType(a, b, c, e, f, g, i, j, k).value();
//    return transpose(res);
//}

template<typename T, typename TSeq>
constexpr Matrix<T, 3, TSeq> adj_matrix(const Matrix<T, 3, TSeq> &mat) {
    auto &&[a, b, c] = mat.data[0];
    auto &&[d, e, f] = mat.data[1];
    auto &&[g, h, i] = mat.data[2];

	using MatrixDelType = MatrixDet<T, 2>;
    Matrix<T, 3, TSeq> res;
    res(0, 0) = +MatrixDelType(e, f, h, i).value();
    res(0, 1) = -MatrixDelType(d, f, g, i).value();
    res(0, 2) = +MatrixDelType(d, e, g, h).value();
    res(1, 0) = -MatrixDelType(b, c, h, i).value();
    res(1, 1) = +MatrixDelType(a, c, g, i).value();
    res(1, 2) = -MatrixDelType(a, b, g, h).value();
    res(2, 0) = +MatrixDelType(b, c, e, f).value();
    res(2, 1) = -MatrixDelType(a, c, d, f).value();
    res(2, 2) = +MatrixDelType(a, b, d, e).value();
    return transpose(res);
}

template<typename T, typename TSeq>
constexpr Matrix<T, 2, TSeq> adj_matrix(const Matrix<T, 2, TSeq> &mat) {
    auto &&[a, b] = mat.data[0];
    auto &&[c, d] = mat.data[1];
	Matrix<T, 2, TSeq> res;
    res(0, 0) = +d;
    res(0, 1) = -c;
    res(1, 0) = -b;
    res(1, 1) = +a;
    return transpose(res);
}

}

template<typename T, size_t N, typename TSeq> requires (N >= 2 && N <= 3)
constexpr Matrix<T, N, TSeq> inverse(const Matrix<T, N, TSeq> &mat) {
    float value = matrix::MatrixDet<T, N>(mat).value();
    if (value == 0.f)
        return Matrix<T, N, TSeq>::identity();

    auto adj_m = matrix::adj_matrix(mat);
    adj_m /= value;
    return adj_m;
}