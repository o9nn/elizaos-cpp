#include "select.hpp"

namespace elizaos {
namespace eliza_client {

bool Select::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Select::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Select::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos
