#ifndef ELIZAOS_CPP_EXTERNAL_TEST_HYBRID_TYPES_AGENT_HPP_
#define ELIZAOS_CPP_EXTERNAL_TEST_HYBRID_TYPES_AGENT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace test_hybrid {

class Agent {
public:
    Agent() = default;
    ~Agent() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "agent"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace test_hybrid
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_TEST_HYBRID_TYPES_AGENT_HPP_
