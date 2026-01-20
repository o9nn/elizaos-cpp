#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

class Logger {
public:
    void info(const std::string& message);
    void debug(const std::string& message);
    void error(const std::string& message, std::optional<std::any> error);
    void warn(const std::string& message);
};


} // namespace elizaos
