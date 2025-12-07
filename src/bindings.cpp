#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <stdexcept>

#include "svd/svd.hpp"

namespace py = pybind11;

namespace {

svd::Matrix matrix_from_array(const py::array_t<double> &array) {
    py::buffer_info info = array.request();
    if (info.ndim != 2) {
        throw std::invalid_argument("Input array must be two-dimensional");
    }
    std::size_t rows = static_cast<std::size_t>(info.shape[0]);
    std::size_t cols = static_cast<std::size_t>(info.shape[1]);
    svd::Matrix matrix(rows, cols);
    const double *data_ptr = static_cast<double *>(info.ptr);
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            matrix(r, c) = data_ptr[r * cols + c];
        }
    }
    return matrix;
}

py::array_t<double> matrix_to_array(const svd::Matrix &matrix) {
    py::array_t<double> array(
        {static_cast<py::ssize_t>(matrix.rows()), static_cast<py::ssize_t>(matrix.cols())});
    auto buffer = array.mutable_unchecked<2>();
    for (std::size_t r = 0; r < matrix.rows(); ++r) {
        for (std::size_t c = 0; c < matrix.cols(); ++c) {
            buffer(r, c) = matrix(r, c);
        }
    }
    return array;
}

} // namespace

PYBIND11_MODULE(svd_bindings, m) {
    m.doc() = "Singular Value Decomposition implemented in C++ and exposed via pybind11";

    m.def(
        "svd",
        [](const py::array_t<double> &input, double tolerance, std::size_t max_sweeps) {
            svd::Matrix matrix = matrix_from_array(input);
            svd::SVDResult result = svd::compute_svd(matrix, tolerance, max_sweeps);
            py::array_t<double> singular_values_array(
                static_cast<py::ssize_t>(result.singular_values.size()));
            auto singular_buffer = singular_values_array.mutable_unchecked<1>();
            for (py::ssize_t i = 0; i < singular_buffer.shape(0); ++i) {
                singular_buffer(i) = result.singular_values[static_cast<std::size_t>(i)];
            }
            return py::make_tuple(
                matrix_to_array(result.U), singular_values_array, matrix_to_array(result.V));
        },
        py::arg("matrix"),
        py::arg("tolerance") = 1e-10,
        py::arg("max_sweeps") = 100,
        R"pbdoc(Compute the thin SVD of a matrix.

Returns a tuple (U, S, V) where U has the same number of rows as the
input matrix, V is square, and S contains the singular values in
decreasing order.)pbdoc");
}
