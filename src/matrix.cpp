#include "svd/matrix.hpp"

#include <stdexcept>

namespace svd {

Matrix::Matrix(std::size_t rows, std::size_t cols)
    : rows_(rows), cols_(cols), data_(rows * cols, 0.0) {}

Matrix::Matrix(std::size_t rows, std::size_t cols, const std::vector<double> &data)
    : rows_(rows), cols_(cols), data_(data) {
    if (data_.size() != rows_ * cols_) {
        throw std::invalid_argument("Matrix data size does not match dimensions");
    }
}

double &Matrix::operator()(std::size_t row, std::size_t col) {
    return data_[row * cols_ + col];
}

double Matrix::operator()(std::size_t row, std::size_t col) const {
    return data_[row * cols_ + col];
}

Matrix transpose(const Matrix &input) {
    Matrix result(input.cols(), input.rows());
    for (std::size_t r = 0; r < input.rows(); ++r) {
        for (std::size_t c = 0; c < input.cols(); ++c) {
            result(c, r) = input(r, c);
        }
    }
    return result;
}

Matrix multiply(const Matrix &lhs, const Matrix &rhs) {
    if (lhs.cols() != rhs.rows()) {
        throw std::invalid_argument("Matrix dimensions do not align for multiplication");
    }
    Matrix result(lhs.rows(), rhs.cols());
    for (std::size_t r = 0; r < lhs.rows(); ++r) {
        for (std::size_t c = 0; c < rhs.cols(); ++c) {
            double sum = 0.0;
            for (std::size_t k = 0; k < lhs.cols(); ++k) {
                sum += lhs(r, k) * rhs(k, c);
            }
            result(r, c) = sum;
        }
    }
    return result;
}

Matrix identity(std::size_t size) {
    Matrix id(size, size);
    for (std::size_t i = 0; i < size; ++i) {
        id(i, i) = 1.0;
    }
    return id;
}

} // namespace svd

