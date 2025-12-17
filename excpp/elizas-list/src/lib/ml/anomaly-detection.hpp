#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".monitoring/logger.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class AnomalyDetector {
public:
    AnomalyDetector();
    void initialize();
    void buildAutoencoder();
    std::future<std::vector<AnomalyResult>> detectAnomalies(const std::vector<std::vector<double>>& data);
    std::future<std::vector<double>> detectWithAutoencoder(const std::vector<std::vector<double>>& data);
    std::future<std::vector<double>> detectWithIsolationForest(const std::vector<std::vector<double>>& data);
    std::vector<double> detectWithDBSCAN(const std::vector<std::vector<double>>& data);
    std::vector<AnomalyResult> ensembleResults(const std::vector<std::vector<double>>& results);

private:
    tf::LayersModel autoencoder_;
    IsolationForest isolationForest_;
    DBSCAN dbscan_;
};
 
} // namespace elizaos
