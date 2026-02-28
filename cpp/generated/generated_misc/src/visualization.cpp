#include "visualization.hpp"

namespace elizaos {
namespace generated_misc {

bool Visualization::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Visualization::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Visualization::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
