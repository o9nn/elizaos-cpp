#ifndef ELIZAOS_CPP_GENERATED_GENERATED_UTILS_INCLUDE_BUN_INSTALLATION_HELPER_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_UTILS_INCLUDE_BUN_INSTALLATION_HELPER_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_utils {

class BunInstallationHelper {
public:
    BunInstallationHelper() = default;
    ~BunInstallationHelper() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "bun_installation_helper"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_utils
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_UTILS_INCLUDE_BUN_INSTALLATION_HELPER_HPP_
