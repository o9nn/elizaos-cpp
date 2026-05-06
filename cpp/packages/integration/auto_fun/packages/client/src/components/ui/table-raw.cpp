#include "table-raw.hpp"

namespace elizaos {
namespace autofun_client {

bool TableRaw::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TableRaw::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TableRaw::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
