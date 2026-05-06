#include "TokenDetail.hpp"

namespace elizaos {
namespace aum_tracker {

bool Tokendetail::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Tokendetail::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Tokendetail::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace aum_tracker
} // namespace elizaos
