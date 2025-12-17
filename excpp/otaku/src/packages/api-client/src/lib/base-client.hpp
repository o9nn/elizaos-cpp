#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types/base.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ApiError {
public:
    ApiError(const std::string& message);
};

    void setAuthToken(const std::string& token);
    void clearAuthToken();


} // namespace elizaos
