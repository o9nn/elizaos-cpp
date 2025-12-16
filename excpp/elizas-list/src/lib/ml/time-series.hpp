#pragma once
#include <any>
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



class TimeSeriesAnalyzer {
public:
    TimeSeriesAnalyzer();
    Promise< forecast(const std::vector<std::any>& historicalData, number = 30 horizon);
    void catch(auto error);
    std::future<std::vector<double>> getLSTMPredictions(const std::vector<std::any>& historicalData, double horizon);
    void trainLSTM(const std::vector<std::any>& historicalData);
    std::future<std::vector<double>> getARIMAPredictions(const std::vector<std::any>& historicalData, double horizon);
    std::vector<double> ensemblePredictions(const std::vector<double>& prophetPreds, const std::vector<double>& lstmPreds, const std::vector<double>& arimaPreds);

private:
    Prophet prophet_;
    tf::LayersModel lstmModel_;
};
 
} // namespace elizaos
