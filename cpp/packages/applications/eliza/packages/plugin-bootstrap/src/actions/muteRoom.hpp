#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_PLUGIN_BOOTSTRAP_SRC_ACTIONS_MUTEROOM_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_PLUGIN_BOOTSTRAP_SRC_ACTIONS_MUTEROOM_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace eliza_plugin_bootstrap {

class Muteroom {
public:
    Muteroom() = default;
    ~Muteroom() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "muteRoom"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace eliza_plugin_bootstrap
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_PLUGIN_BOOTSTRAP_SRC_ACTIONS_MUTEROOM_HPP_
