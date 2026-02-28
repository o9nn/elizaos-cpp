#ifndef ELIZAOS_CPP_GENERATED_GENERATED_DOCS_INCLUDE_DOCS_LOADER_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_DOCS_INCLUDE_DOCS_LOADER_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_docs {

class DocsLoader {
public:
    DocsLoader() = default;
    ~DocsLoader() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "docs_loader"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_docs
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_DOCS_INCLUDE_DOCS_LOADER_HPP_
