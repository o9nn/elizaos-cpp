#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ML_TIME-SERIES-DECOMPOSITION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ML_TIME-SERIES-DECOMPOSITION_H
#include "core.h"
#include "@tensorflow/tfjs-node.h"
#include "time-series-decomposition.h"
#include "wavelets.h"
#include "../monitoring/logger.h"

class TimeSeriesDecomposer;

class TimeSeriesDecomposer : public object, public std::enable_shared_from_this<TimeSeriesDecomposer> {
public:
    using std::enable_shared_from_this<TimeSeriesDecomposer>::shared_from_this;
    std::shared_ptr<STL> stl;

    std::shared_ptr<WaveletTransform> wavelet;

    TimeSeriesDecomposer();
    virtual std::shared_ptr<Promise<std::shared_ptr<DecompositionResult>>> decompose(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config);
    virtual std::shared_ptr<Promise<std::shared_ptr<STLResult>>> performSTLDecomposition(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config);
    virtual std::shared_ptr<Promise<std::shared_ptr<WaveletResult>>> performWaveletDecomposition(array<double> timeSeries, std::shared_ptr<DecompositionConfig> config);
    virtual std::shared_ptr<Promise<std::shared_ptr<EMDResult>>> performEMD(array<double> timeSeries);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Changepoint>>>> detectChangepoints(array<double> timeSeries, std::shared_ptr<DecompositionComponents> components);
    virtual std::shared_ptr<Promise<std::shared_ptr<ComponentForecasts>>> generateComponentForecasts(std::shared_ptr<DecompositionComponents> components, std::shared_ptr<DecompositionConfig> config);
};

#endif
