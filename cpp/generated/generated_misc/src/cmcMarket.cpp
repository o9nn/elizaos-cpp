#include "cmcMarket.hpp"

namespace elizaos {
namespace generated_misc {

bool Cmcmarket::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Cmcmarket::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Cmcmarket::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
