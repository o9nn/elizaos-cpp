#include "s3Client.hpp"

namespace elizaos {
namespace generated_api {

bool S3client::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void S3client::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json S3client::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_api
} // namespace elizaos
