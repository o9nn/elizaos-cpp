#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CodeBlockCodeProps {
    std::string code;
    std::optional<std::string> language;
    std::optional<std::string> className;
    std::optional<std::variant<'user', 'agent'>> variant;
};


} // namespace elizaos
