#include "suite.hpp"

namespace elizaos {
namespace generated_ui {

bool Suite::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Suite::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Suite::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_ui
} // namespace elizaos
