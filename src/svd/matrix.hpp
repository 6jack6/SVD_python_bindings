#pragma once

#include <cstddef>
#include <vector>

namespace svd {

class Matrix {
public:
    Matrix() = default;
    Matrix(std::size_t rows, std::size_t cols);
    Matrix(std::size_t rows, std::size_t cols, const std::vector<double> &data);

    double &operator()(std::size_t row, std::size_t col);
    double operator()(std::size_t row, std::size_t col) const;

    std::size_t rows() const { return rows_; }
    std::size_t cols() const { return cols_; }

    const std::vector<double> &data() const { return data_; }
    std::vector<double> &data() { return data_; }

private:
    std::size_t rows_{0};
    std::size_t cols_{0};
    std::vector<double> data_{};
};

Matrix transpose(const Matrix &input);
Matrix multiply(const Matrix &lhs, const Matrix &rhs);
Matrix identity(std::size_t size);

} // namespace svd

