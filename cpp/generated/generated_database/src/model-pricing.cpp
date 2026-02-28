#include "model-pricing.hpp"

namespace elizaos {
namespace generated_database {

bool ModelPricing::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ModelPricing::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ModelPricing::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_database
} // namespace elizaos
