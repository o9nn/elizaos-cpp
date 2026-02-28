#include "TxToast.hpp"

namespace elizaos {
namespace autofun_client {

bool Txtoast::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Txtoast::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Txtoast::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
