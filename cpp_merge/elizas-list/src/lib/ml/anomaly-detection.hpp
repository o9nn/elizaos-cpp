#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ML_ANOMALY_DETECTION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ML_ANOMALY_DETECTION_H
#include "core.hpp"
// External dependency removed
#include "isolation-forest.h"
#include "density-clustering.h"
#include "../monitoring/logger.h"

class AnomalyDetector;

class AnomalyDetector : public object, public std::enable_shared_from_this<AnomalyDetector> {
public:
    using std::enable_shared_from_this<AnomalyDetector>::shared_from_this;
    std::shared_ptr<tf::LayersModel> autoencoder;

    std::shared_ptr<IsolationForest> isolationForest;

    std::shared_ptr<DBSCAN> dbscan;

    double threshold = 0.95;

    AnomalyDetector();
    virtual void initialize();
    virtual void buildAutoencoder();
    virtual std::shared_ptr<Promise<array<std::shared_ptr<AnomalyResult>>>> detectAnomalies(array<array<double>> data);
    virtual std::shared_ptr<Promise<array<double>>> detectWithAutoencoder(array<array<double>> data);
    virtual std::shared_ptr<Promise<array<double>>> detectWithIsolationForest(array<array<double>> data);
    virtual array<double> detectWithDBSCAN(array<array<double>> data);
    virtual array<std::shared_ptr<AnomalyResult>> ensembleResults(array<array<double>> results);
};

#endif
