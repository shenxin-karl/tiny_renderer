#pragma once

template<typename T, size_t N, typename = std::make_index_sequence<N*N>>
struct Matrix;

template<typename T, size_t N, size_t...I>
struct Matrix<T, N, std::index_sequence<I...>> {
    T data[N][N];
public:
    constexpr Matrix() noexcept : Matrix(static_cast<T>((I / N) == (I % N))...) {}
    constexpr Matrix(identity_t<T, I>... scalars) noexcept : data{ scalars... } {}
    constexpr Matrix(const Matrix &) noexcept = default;
    constexpr Matrix &operator=(const Matrix &) noexcept = default;

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

    constexpr Matrix *operator/=(const T &f) noexcept {
        (((*this)[I] /= f), ...);
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
    constexpr T &operator[](size_t idx) noexcept {
        return (&data[0][0])[idx];
    }

    constexpr const T &operator[](size_t idx) const noexcept {
		return (&data[0][0])[idx];
    }
};

template<typename T, size_t N, size_t...I>
inline std::ostream &operator<<(std::ostream &os, const Matrix<T, N, std::index_sequence<I...>> &mat) noexcept {
    for (auto &row : mat.data) {
        for (T f : row) 
            os << f << " ";
        os << std::endl;
    }
    return os;
}

using mat3 = Matrix<float, 3>;
using mat4 = Matrix<float, 4>;
using mat2 = Matrix<float, 2>;