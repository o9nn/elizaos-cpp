#include "embedding-seed.hpp"

namespace elizaos {
namespace generated_misc {

bool EmbeddingSeed::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void EmbeddingSeed::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json EmbeddingSeed::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
