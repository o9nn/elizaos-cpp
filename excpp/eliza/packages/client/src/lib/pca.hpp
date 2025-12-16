#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


  for (let i = 0; i < dim; i++) mean[i] /= data.length;
  const centered = data.map((v) => v.map((val, idx) => val - mean[idx]));
  // covariance matrix
  const cov = Array.from({ length: dim }, () => Array(dim).fill(0));
  for (const vec of centered) {
    for (let i = 0; i < dim; i++) {
      for (let j = 0; j < dim; j++) {
        cov[i][j] += vec[i] * vec[j];
      }
    }
  }
  for (let i = 0; i < dim; i++) {
    for (let j = 0; j < dim; j++) {
      cov[i][j] /= data.length - 1;
    }
  }
  const eigenvectors: number[][] = [];
  let matrix = cov.map((row) => row.slice());
  for (let k = 0; k < dims; k++) {
    // start with a deterministic unit vector for stability
    let vec = Array(dim).fill(1 / Math.sqrt(dim));
    for (let iter = 0; iter < 50; iter++) {
      const next = multiplyMatrixVector(matrix, vec);
      const norm = Math.sqrt(next.reduce((a, b) => a + b * b, 0));
      if (norm === 0) break;
      vec = next.map((v) => v / norm);
    }
    eigenvectors.push(vec.slice());
    const lambda = dot(vec, multiplyMatrixVector(matrix, vec));
    for (let i = 0; i < dim; i++) {
      for (let j = 0; j < dim; j++) {
        matrix[i][j] -= lambda * vec[i] * vec[j];
      }
    }
  }
  return centered.map((vec) => eigenvectors.map((ev) => dot(vec, ev)));
}





} // namespace elizaos
