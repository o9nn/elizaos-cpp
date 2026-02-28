#ifndef ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_FLORENCE2_MODEL_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_FLORENCE2_MODEL_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_database {

class Florence2Model {
public:
    Florence2Model() = default;
    ~Florence2Model() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "florence2_model"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_database
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_DATABASE_INCLUDE_FLORENCE2_MODEL_HPP_
