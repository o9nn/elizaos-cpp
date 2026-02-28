#include "tos-provider.hpp"

namespace elizaos {
namespace autofun_client {

bool TosProvider::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TosProvider::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TosProvider::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
