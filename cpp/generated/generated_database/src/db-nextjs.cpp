#include "db-nextjs.hpp"

namespace elizaos {
namespace generated_database {

bool DbNextjs::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void DbNextjs::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json DbNextjs::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_database
} // namespace elizaos
