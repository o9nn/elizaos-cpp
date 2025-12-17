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
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class MorphoError {
public:
    MorphoError(MorphoErrorCode code, const std::string& message, std::optional<std::any> details, std::optional<std::vector<std::string>> suggestions, std::optional<MatchingImpact> matchingImpact, std::optional<std::vector<std::string>> fallbackOptions);

private:
    MorphoErrorCode code_;
};

class ErrorHandler {
public:
    static MorphoError handle(const std::any& error);
    static MorphoErrorResponse createResponse(MorphoError error);
    static bool isRecoverableError(MorphoError error);
    static std::string getSuggestion(MorphoError error);
};


} // namespace elizaos
