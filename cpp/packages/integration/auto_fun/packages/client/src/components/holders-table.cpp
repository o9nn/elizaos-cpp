#include "holders-table.hpp"

namespace elizaos {
namespace autofun_client {

bool HoldersTable::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void HoldersTable::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json HoldersTable::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
