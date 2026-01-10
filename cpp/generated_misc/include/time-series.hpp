#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ML_TIME-SERIES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ML_TIME-SERIES_H
#include "core.h"
#include "@tensorflow/tfjs-node.h"
#include "prophet-js.h"
#include "statistics.js.h"
#include "../monitoring/logger.h"

class TimeSeriesAnalyzer;

class TimeSeriesAnalyzer : public object, public std::enable_shared_from_this<TimeSeriesAnalyzer> {
public:
    using std::enable_shared_from_this<TimeSeriesAnalyzer>::shared_from_this;
    std::shared_ptr<Prophet> prophet;

    std::shared_ptr<tf::LayersModel> lstmModel;

    TimeSeriesAnalyzer();
    virtual std::shared_ptr<Promise<object>> forecast(array<object> historicalData, double horizon = 30);
    virtual std::shared_ptr<Promise<array<double>>> getLSTMPredictions(array<object> historicalData, double horizon);
    virtual void trainLSTM(array<object> historicalData);
    virtual std::shared_ptr<Promise<array<double>>> getARIMAPredictions(array<object> historicalData, double horizon);
    virtual array<double> ensemblePredictions(array<double> prophetPreds, array<double> lstmPreds, array<double> arimaPreds);
};

#endif
