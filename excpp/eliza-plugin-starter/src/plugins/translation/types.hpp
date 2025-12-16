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

;

{ ElizaServiceType as ServiceType };

struct TranslationConfig {
    { provider;
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


interface TranslationActionContent extends Content {
  text: string;
}

interface TranslationEvalContent extends Content {
  text: string;
}

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


interface TranslationService extends Service {
  translate(text: string, targetLang: string, sourceLang?: string): Promise<TranslationServiceResponse>;
}

struct TranslationServiceResponse {
    bool success;
    std::optional<std::string> translation;
    std::optional<std::string> error;
    std::optional<{> metadata;
    std::string model;
    double confidence;
    double tokensUsed;
};


} // namespace elizaos
