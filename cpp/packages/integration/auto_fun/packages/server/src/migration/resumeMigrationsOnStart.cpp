#include "resumeMigrationsOnStart.hpp"

namespace elizaos {
namespace autofun_server {

bool Resumemigrationsonstart::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Resumemigrationsonstart::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Resumemigrationsonstart::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos
