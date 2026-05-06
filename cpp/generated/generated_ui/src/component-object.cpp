#include "component-object.hpp"

namespace elizaos {
namespace generated_ui {

bool ComponentObject::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ComponentObject::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ComponentObject::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_ui
} // namespace elizaos
