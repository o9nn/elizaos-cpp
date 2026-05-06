#include "copy-template.hpp"

namespace elizaos {
namespace eliza_cli {

bool CopyTemplate::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void CopyTemplate::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json CopyTemplate::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos
