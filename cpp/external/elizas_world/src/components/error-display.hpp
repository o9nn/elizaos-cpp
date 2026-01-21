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



struct ErrorDisplayProps {
    std::variant<Error, unknown> error;
};

void ErrorDisplay(ErrorDisplayProps { error }); 
} // namespace elizaos
