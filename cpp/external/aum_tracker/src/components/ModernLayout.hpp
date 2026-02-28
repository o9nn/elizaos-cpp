#ifndef ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_COMPONENTS_MODERNLAYOUT_HPP_
#define ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_COMPONENTS_MODERNLAYOUT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace aum_tracker {

class Modernlayout {
public:
    Modernlayout() = default;
    ~Modernlayout() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "ModernLayout"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace aum_tracker
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_COMPONENTS_MODERNLAYOUT_HPP_
