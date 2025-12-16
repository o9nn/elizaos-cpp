#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TranslationEvalContent {
    std::string text;
    std::optional<{> translationData;
    std::string sourceText;
    std::string targetText;
    std::string sourceLang;
    std::string targetLang;
    double confidence;
};

struct TranslationEvalResponse {
    bool isValid;
    std::string reason;
    std::optional<std::vector<std::string>> suggestions;
};

      // Quality checks

      // Language compatibility check


} // namespace elizaos
