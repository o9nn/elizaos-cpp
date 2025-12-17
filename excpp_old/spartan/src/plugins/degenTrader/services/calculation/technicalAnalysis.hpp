#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".base/BaseTradeService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TechnicalAnalysisService {
public:
    void calculateTechnicalSignals(const std::any& marketData);
};


} // namespace elizaos
