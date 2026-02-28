#include "page-title.hpp"

namespace elizaos {
namespace eliza_client {

bool PageTitle::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void PageTitle::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json PageTitle::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos
