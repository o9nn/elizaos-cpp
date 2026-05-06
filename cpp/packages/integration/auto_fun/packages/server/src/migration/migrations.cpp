#include "migrations.hpp"

namespace elizaos {
namespace autofun_server {

bool Migrations::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Migrations::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Migrations::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos
