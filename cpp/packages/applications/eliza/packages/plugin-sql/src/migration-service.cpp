#include "migration-service.hpp"

namespace elizaos {
namespace eliza_plugin_sql {

bool MigrationService::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void MigrationService::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json MigrationService::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_sql
} // namespace elizaos
