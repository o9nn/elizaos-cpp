#ifndef ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_CLIENT_SRC_COMPONENTS_GRID_LIST_SWITCHER_HPP_
#define ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_CLIENT_SRC_COMPONENTS_GRID_LIST_SWITCHER_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace autofun_client {

class GridListSwitcher {
public:
    GridListSwitcher() = default;
    ~GridListSwitcher() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "grid_list_switcher"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace autofun_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_CLIENT_SRC_COMPONENTS_GRID_LIST_SWITCHER_HPP_
