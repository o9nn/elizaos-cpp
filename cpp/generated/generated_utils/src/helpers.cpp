#include "helpers.hpp"

namespace elizaos {
namespace generated_utils {

bool Helpers::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Helpers::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Helpers::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_utils
} // namespace elizaos
