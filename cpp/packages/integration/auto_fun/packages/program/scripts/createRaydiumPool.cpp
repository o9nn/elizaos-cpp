#include "createRaydiumPool.hpp"

namespace elizaos {
namespace autofun_program {

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

} // namespace autofun_program
} // namespace elizaos
