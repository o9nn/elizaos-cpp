#pragma once
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



class TokenValidationService {
public:
    Promise< validateTokenForTrading(const std::string& tokenAddress);
    Promise< fetchTokenMetadata(const std::string& tokenAddress);

private:
    bool isValid_;
};


} // namespace elizaos
