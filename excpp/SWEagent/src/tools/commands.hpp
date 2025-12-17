#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Command definitions for tools
 * Converted from sweagent/tools/commands.py
 */

/**
 * Extract keys from a format string
 */
std::unordered_set<std::string> extractKeys(const std::string& formatString);

/**
 * Command argument definition
 */
struct IArgument {
    std::string name;
    std::string type;
    std::string description;
    bool required;
    std::optional<std::optional<std::vector<std::string>>> enum;
    std::string argumentFormat;
};

/**
 * Command argument implementation
 */
class ArgumentImpl {
public:
    ArgumentImpl(std::optional<std::any> config);
    void validateArgumentFormat();

private:
    std::string name_;
    std::string type_;
    std::string description_;
    bool required_;
    std::string argumentFormat_;
};

/**
 * Command definition
 */
class Command {
public:
    Command(std::optional<std::any> config);
    std::string invokeFormat() const;
    std::unordered_map<std::string, std::any> getFunctionCallingTool();
    void validateArguments();

private:
    std::string name_;
    std::string docstring_;
    std::string signature_;
    std::vector<IArgument> arguments_;
};

// Export ArgumentImpl as Argument for tests that expect it as a class

} // namespace elizaos
