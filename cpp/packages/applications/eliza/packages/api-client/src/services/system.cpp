#include "system.hpp"

namespace elizaos {
namespace eliza_api_client {

bool System::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void System::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json System::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_api_client
} // namespace elizaos
