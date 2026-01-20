#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_LIB_PCA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_LIB_PCA_H
#include "core.h"

array<array<double>> computePca(array<array<double>> data, double dims = 2);

array<double> multiplyMatrixVector(array<array<double>> mat, array<double> vec);

double dot(array<double> a, array<double> b);

#endif
