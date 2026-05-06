#ifndef ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_06_UI_NAVIGATION_CY_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_06_UI_NAVIGATION_CY_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_testing {

class Test06UiNavigationCy {
public:
    Test06UiNavigationCy() = default;
    ~Test06UiNavigationCy() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "06_ui_navigation_cy"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_testing
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_06_UI_NAVIGATION_CY_HPP_
