#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_CLI_SRC_UTILS_REGISTRY_CONSTANTS_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_CLI_SRC_UTILS_REGISTRY_CONSTANTS_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace eliza_cli {

class Constants {
public:
    Constants() = default;
    ~Constants() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "constants"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace eliza_cli
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_CLI_SRC_UTILS_REGISTRY_CONSTANTS_HPP_
