#include "docusaurus-example.hpp"

namespace elizaos {
namespace autofun_docs {

bool DocusaurusExample::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void DocusaurusExample::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json DocusaurusExample::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_docs
} // namespace elizaos
