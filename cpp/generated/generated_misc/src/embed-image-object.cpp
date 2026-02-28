#include "embed-image-object.hpp"

namespace elizaos {
namespace generated_misc {

bool EmbedImageObject::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void EmbedImageObject::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json EmbedImageObject::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
