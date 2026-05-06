#include "use-query-hooks.hpp"

namespace elizaos {
namespace eliza_client {

bool UseQueryHooks::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UseQueryHooks::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UseQueryHooks::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos
