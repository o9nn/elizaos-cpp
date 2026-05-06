#include "readmeUtils.hpp"

namespace elizaos {
namespace generated_utils {

bool Readmeutils::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Readmeutils::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Readmeutils::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_utils
} // namespace elizaos
