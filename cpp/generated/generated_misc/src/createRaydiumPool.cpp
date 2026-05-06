#include "createRaydiumPool.hpp"

namespace elizaos {
namespace generated_misc {

bool Createraydiumpool::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Createraydiumpool::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Createraydiumpool::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
