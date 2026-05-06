#ifndef ELIZAOS_CPP_EXTERNAL_SANDBOX_TEMPLATE_CLOUD_SRC_APP_LAYOUT_HPP_
#define ELIZAOS_CPP_EXTERNAL_SANDBOX_TEMPLATE_CLOUD_SRC_APP_LAYOUT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace sandbox_template_cloud {

class Layout {
public:
    Layout() = default;
    ~Layout() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "layout"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace sandbox_template_cloud
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_SANDBOX_TEMPLATE_CLOUD_SRC_APP_LAYOUT_HPP_
