#include "uuid-utils.hpp"

namespace elizaos {
namespace generated_utils {

bool UuidUtils::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UuidUtils::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UuidUtils::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_utils
} // namespace elizaos
