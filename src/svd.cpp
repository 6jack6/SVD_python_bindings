#include "svd/svd.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace svd {
namespace {

struct EigenResult {
    std::vector<double> values;
    Matrix vectors;
};

EigenResult jacobi_eigenvalue(const Matrix &input, double tolerance, std::size_t max_sweeps) {
    if (input.rows() != input.cols()) {
        throw std::invalid_argument("Jacobi eigenvalue decomposition requires a square matrix");
    }

    const std::size_t n = input.rows();
    Matrix eigenvectors = identity(n);
    Matrix diagonalized = input; // copy

    for (std::size_t sweep = 0; sweep < max_sweeps; ++sweep) {
        bool converged = true;
        for (std::size_t p = 0; p < n - 1; ++p) {
            for (std::size_t q = p + 1; q < n; ++q) {
                double apq = diagonalized(p, q);
                if (std::abs(apq) <= tolerance) {
                    continue;
                }
                converged = false;
                double app = diagonalized(p, p);
                double aqq = diagonalized(q, q);
                double phi = 0.5 * std::atan2(2.0 * apq, aqq - app);
                double c = std::cos(phi);
                double s = std::sin(phi);

                for (std::size_t k = 0; k < n; ++k) {
                    if (k == p || k == q) {
                        continue;
                    }
                    double dkp = diagonalized(k, p);
                    double dkq = diagonalized(k, q);
                    double new_dkp = c * dkp - s * dkq;
                    double new_dkq = s * dkp + c * dkq;
                    diagonalized(k, p) = new_dkp;
                    diagonalized(p, k) = new_dkp;
                    diagonalized(k, q) = new_dkq;
                    diagonalized(q, k) = new_dkq;
                }

                double new_app = c * c * app - 2.0 * s * c * apq + s * s * aqq;
                double new_aqq = s * s * app + 2.0 * s * c * apq + c * c * aqq;
                diagonalized(p, p) = new_app;
                diagonalized(q, q) = new_aqq;
                diagonalized(p, q) = 0.0;
                diagonalized(q, p) = 0.0;

                for (std::size_t k = 0; k < n; ++k) {
                    double vip = eigenvectors(k, p);
                    double viq = eigenvectors(k, q);
                    eigenvectors(k, p) = c * vip - s * viq;
                    eigenvectors(k, q) = s * vip + c * viq;
                }
            }
        }
        if (converged) {
            break;
        }
    }

    std::vector<double> eigenvalues(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        eigenvalues[i] = diagonalized(i, i);
    }

    return {std::move(eigenvalues), std::move(eigenvectors)};
}

Matrix reorder_columns(const Matrix &input, const std::vector<std::size_t> &order) {
    if (order.size() != input.cols()) {
        throw std::invalid_argument("Order length must match the number of matrix columns");
    }
    Matrix result(input.rows(), input.cols());
    for (std::size_t idx = 0; idx < order.size(); ++idx) {
        std::size_t column_index = order[idx];
        for (std::size_t row = 0; row < input.rows(); ++row) {
            result(row, idx) = input(row, column_index);
        }
    }
    return result;
}

} // namespace

SVDResult compute_svd(const Matrix &input, double tolerance, std::size_t max_sweeps) {
    if (input.rows() == 0 || input.cols() == 0) {
        throw std::invalid_argument("Input matrix must not be empty");
    }

    Matrix normal_matrix = multiply(transpose(input), input);
    EigenResult eigen = jacobi_eigenvalue(normal_matrix, tolerance, max_sweeps);

    std::vector<double> singular_values(eigen.values.size(), 0.0);
    for (std::size_t i = 0; i < eigen.values.size(); ++i) {
        double clamped = std::max(eigen.values[i], 0.0);
        singular_values[i] = std::sqrt(clamped);
    }

    std::vector<std::size_t> order(singular_values.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](std::size_t lhs, std::size_t rhs) {
        return singular_values[lhs] > singular_values[rhs];
    });

    Matrix ordered_V = reorder_columns(eigen.vectors, order);
    std::vector<double> ordered_singulars(order.size(), 0.0);
    for (std::size_t i = 0; i < order.size(); ++i) {
        ordered_singulars[i] = singular_values[order[i]];
    }

    Matrix U = multiply(input, ordered_V);
    const double sigma_tolerance = 1e-12;
    for (std::size_t col = 0; col < ordered_singulars.size(); ++col) {
        double sigma = ordered_singulars[col];
        if (sigma <= sigma_tolerance) {
            for (std::size_t row = 0; row < U.rows(); ++row) {
                U(row, col) = 0.0;
            }
            continue;
        }
        for (std::size_t row = 0; row < U.rows(); ++row) {
            U(row, col) /= sigma;
        }
    }

    return {std::move(U), std::move(ordered_singulars), std::move(ordered_V)};
}

} // namespace svd
