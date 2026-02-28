#include "trading.hpp"

namespace elizaos {
namespace generated_trade {

bool Trading::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Trading::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Trading::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_trade
} // namespace elizaos
