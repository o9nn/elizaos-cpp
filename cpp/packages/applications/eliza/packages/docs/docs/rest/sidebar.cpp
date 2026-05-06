#include "sidebar.hpp"

namespace elizaos {
namespace eliza_docs {

bool Sidebar::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Sidebar::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Sidebar::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_docs
} // namespace elizaos
