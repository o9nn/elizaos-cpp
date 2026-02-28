#include "main.hpp"

namespace elizaos {
namespace eliza_client {

bool Main::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Main::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Main::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos
