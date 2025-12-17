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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TranslationConfig {
    std::string apiKey;
    std::optional<std::string> baseUrl;
    std::optional<std::string> model;
};

struct TranslationData {
    std::string sourceText;
    std::string targetText;
    std::string sourceLang;
    std::string targetLang;
    std::optional<double> confidence;
};

struct TranslationEvalResponse {
    bool success;
    std::string response;
    std::optional<double> confidence;
};

struct TranslationProviderResponse {
    bool success;
    std::optional<TranslationData> data;
    std::optional<std::string> error;
};

struct TranslationServiceConfig {
    std::optional<std::string> model;
    std::optional<double> temperature;
    std::optional<double> maxTokens;
};

struct TranslationServiceResponse {
    bool success;
    std::optional<std::string> translation;
    std::optional<std::string> error;
    std::string model;
    double confidence;
    double tokensUsed;
};


} // namespace elizaos
