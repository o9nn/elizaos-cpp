#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "commands.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Tool bundle configuration
 * Converted from sweagent/tools/bundle.py
 */

/**
 * Bundle configuration
 */
struct BundleConfig {
    std::optional<std::string> stateCommand;
};

/**
 * Tool bundle
 */
class Bundle {
public:
    Bundle(std::optional<std::any> config);
    void validateTools();
    std::string stateCommand() const;
    BundleConfig config() const;
    std::vector<Command> commands() const;

private:
    std::string path_;
    std::vector<std::string> hiddenTools_;
};


} // namespace elizaos
