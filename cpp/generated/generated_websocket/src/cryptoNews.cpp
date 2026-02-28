#include "cryptoNews.hpp"

namespace elizaos {
namespace generated_websocket {

bool Cryptonews::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Cryptonews::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Cryptonews::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_websocket
} // namespace elizaos
