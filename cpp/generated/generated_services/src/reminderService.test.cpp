#include "reminderService.test.hpp"

namespace elizaos {
namespace generated_services {

bool ReminderserviceTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ReminderserviceTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ReminderserviceTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_services
} // namespace elizaos
