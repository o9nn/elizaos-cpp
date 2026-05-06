#include "update-form.hpp"

namespace elizaos {
namespace generated_misc {

bool UpdateForm::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UpdateForm::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UpdateForm::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
