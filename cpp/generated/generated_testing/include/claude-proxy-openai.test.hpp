#ifndef ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_CLAUDE_PROXY_OPENAI_TEST_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_CLAUDE_PROXY_OPENAI_TEST_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_testing {

class ClaudeProxyOpenaiTest {
public:
    ClaudeProxyOpenaiTest() = default;
    ~ClaudeProxyOpenaiTest() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "claude_proxy_openai_test"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_testing
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_CLAUDE_PROXY_OPENAI_TEST_HPP_
