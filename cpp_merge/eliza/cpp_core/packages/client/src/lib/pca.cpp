#include "pca.hpp"
#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::vector<double>> computePca(const std::vector<std::vector<double>>& data, auto dims) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (data.size() == 0) return [];
    const auto dim = data[0].size();
    const auto mean = Array(dim).fill(0);
    for (const auto& vec : data)
        for (int i = 0; i < dim; i++) mean[i] += vec[i];
    }
    for (int i = 0; i < dim; i++) mean[i] /= data.size();
    const auto centered = data.std::map[&]((v) { return v.std::map[&]((val, idx) { return val - mean[idx])); }; };
    // covariance matrix
    const auto cov = Array.from[&]({ length: dim }, () { return Array(dim).fill(0)); };
    for (const auto& vec : centered)
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                cov[i][j] += vec[i] * vec[j];
            }
        }
    }
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            cov[i][j] /= data.size() - 1;
        }
    }
    const std::vector<std::vector<double>> eigenvectors = [];
    auto matrix = cov.std::map[&]((row) { return row.slice()); };
    for (int k = 0; k < dims; k++) {
        // start with a deterministic unit std::vector for stability
        auto vec = Array(dim).fill(1 / Math.sqrt(dim));
        for (int iter = 0; iter < 50; iter++) {
            const auto next = multiplyMatrixVector(matrix, vec);
            const auto norm = Math.sqrt[&](next.reduce((a, b) { return a + b * b, 0)); };
            if (norm == 0) break;
            vec = next.std::map[&]((v) { return v / norm); };
        }
        eigenvectors.push_back(vec.slice());
        const auto lambda = dot(vec, multiplyMatrixVector(matrix, vec));
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                matrix[i][j] -= lambda * vec[i] * vec[j];
            }
        }
    }
    return centered.std::map[&]((vec) { return eigenvectors.std::map[&]((ev) { return dot(vec, ev))); }; };

}

std::vector<double> multiplyMatrixVector(const std::vector<std::vector<double>>& mat, const std::vector<double>& vec) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return mat.std::map[&]((row) { return row.reduce[&]((sum, val, i) { return sum + val * vec[i], 0)); }; };

}

double dot(const std::vector<double>& a, const std::vector<double>& b) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return a.reduce[&]((sum, val, i) { return sum + val * b[i], 0); };

}

} // namespace elizaos
