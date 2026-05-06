#ifndef ELIZAOS_CPP_EXTERNAL_MOBILE_CONSTANTS_COLORS_HPP_
#define ELIZAOS_CPP_EXTERNAL_MOBILE_CONSTANTS_COLORS_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace mobile {

class Colors {
public:
    Colors() = default;
    ~Colors() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "Colors"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace mobile
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_MOBILE_CONSTANTS_COLORS_HPP_
