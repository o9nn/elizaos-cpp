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



struct Phase {
    std::string name;
    std::string icon;
    std::string name;
    std::variant<"done", "in-progress", "not-started"> progress;
};

// Helper function to calculate progress percentage

  // Give full weight to done tasks, half weight to in-progress tasks

// Helper function to get task icon


} // namespace elizaos
