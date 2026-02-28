#include "attachment.hpp"

namespace elizaos {
namespace generated_misc {

bool Attachment::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Attachment::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Attachment::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
