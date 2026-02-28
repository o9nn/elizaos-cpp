#include "act_reg_confirmemail.hpp"

namespace elizaos {
namespace generated_misc {

bool ActRegConfirmemail::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ActRegConfirmemail::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ActRegConfirmemail::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
