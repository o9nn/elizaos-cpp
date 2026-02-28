#include "page.hpp"

namespace elizaos {
namespace sandbox_template_cloud {

bool Page::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Page::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Page::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace sandbox_template_cloud
} // namespace elizaos
