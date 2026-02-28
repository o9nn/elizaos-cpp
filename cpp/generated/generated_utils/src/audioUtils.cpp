#include "audioUtils.hpp"

namespace elizaos {
namespace generated_utils {

bool Audioutils::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Audioutils::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Audioutils::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_utils
} // namespace elizaos
