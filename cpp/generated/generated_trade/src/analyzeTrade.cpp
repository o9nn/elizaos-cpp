#include "analyzeTrade.hpp"

namespace elizaos {
namespace generated_trade {

bool Analyzetrade::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Analyzetrade::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Analyzetrade::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_trade
} // namespace elizaos
