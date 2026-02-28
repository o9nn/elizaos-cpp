#ifndef ELIZAOS_CPP_EXTERNAL_SANDBOX_TEMPLATE_CLOUD_NEXT_HPP_
#define ELIZAOS_CPP_EXTERNAL_SANDBOX_TEMPLATE_CLOUD_NEXT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace sandbox_template_cloud {

class Next {
public:
    Next() = default;
    ~Next() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "next"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace sandbox_template_cloud
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_SANDBOX_TEMPLATE_CLOUD_NEXT_HPP_
