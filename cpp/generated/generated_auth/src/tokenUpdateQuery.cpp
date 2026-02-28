#include "tokenUpdateQuery.hpp"

namespace elizaos {
namespace generated_auth {

bool Tokenupdatequery::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Tokenupdatequery::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Tokenupdatequery::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_auth
} // namespace elizaos
