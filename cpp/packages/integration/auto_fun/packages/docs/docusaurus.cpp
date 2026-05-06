#include "docusaurus.hpp"

namespace elizaos {
namespace autofun_docs {

bool Docusaurus::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Docusaurus::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Docusaurus::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_docs
} // namespace elizaos
