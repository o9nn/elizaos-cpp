#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ClankerError {
public:
    ClankerError(ErrorCode code, const std::string& message, std::optional<std::any> details, std::optional<std::vector<std::string>> suggestions);
    ErrorResponse toResponse();

private:
    ErrorCode code_;
};

ErrorResponse handleError(unknown error);

bool validateAddress(const std::string& address);

bool validateAmount(const std::string& amount);

} // namespace elizaos
