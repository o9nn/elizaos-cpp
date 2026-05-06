#ifndef ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_TEST_SANDBOX_GENERATION_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_TEST_SANDBOX_GENERATION_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_database {

class TestSandboxGeneration {
public:
    TestSandboxGeneration() = default;
    ~TestSandboxGeneration() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "test_sandbox_generation"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_database
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_TEST_SANDBOX_GENERATION_HPP_
