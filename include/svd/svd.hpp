#pragma once

#include <cstddef>
#include <vector>

#include "svd/matrix.hpp"

namespace svd {

struct SVDResult {
    Matrix U;
    std::vector<double> singular_values;
    Matrix V;
};

SVDResult compute_svd(const Matrix &input, double tolerance = 1e-10, std::size_t max_sweeps = 100);

} // namespace svd

