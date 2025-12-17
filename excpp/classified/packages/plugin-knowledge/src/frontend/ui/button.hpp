#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ButtonProps {
    ReactNode children;
    std::optional<std::string> variant;
    std::optional<std::string> size;
    std::optional<std::string> className;
    std::optional<bool> disabled;
    std::optional<std::string> title;
    std::optional<std::string> type;
};

void Button(auto variant, auto size, auto className, auto onClick, auto disabled, auto title, auto type);

} // namespace elizaos
