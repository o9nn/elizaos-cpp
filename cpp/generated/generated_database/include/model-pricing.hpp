#ifndef ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_MODEL_PRICING_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_MODEL_PRICING_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_database {

class ModelPricing {
public:
    ModelPricing() = default;
    ~ModelPricing() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "model_pricing"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_database
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_MODEL_PRICING_HPP_
