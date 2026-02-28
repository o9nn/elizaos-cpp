#include "getDaoTrades.hpp"

namespace elizaos {
namespace generated_trade {

bool Getdaotrades::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Getdaotrades::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Getdaotrades::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_trade
} // namespace elizaos
