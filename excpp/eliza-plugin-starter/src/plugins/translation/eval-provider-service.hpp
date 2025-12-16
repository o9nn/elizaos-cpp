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


const translationEvaluator: Evaluator = {
  name: "VALIDATE_TRANSLATION",

  similes: [
    "CHECK_TRANSLATION",
    "TRANSLATION_QUALITY",
    "LANGUAGE_VERIFICATION",
  ],

  description:
    "Validates translation quality, language detection, and provides improvement suggestions",

  validate: async (
    runtime: IAgentRuntime,
    message: Memory,
  ): Promise<boolean> => {
    const content = message.content as TranslationEvalContent;
    return (
      content.translationData !== undefined ||
      /translate|translation|to english|to spanish/i.test(content.text)
    );
  },

  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
  ): Promise<TranslationEvalResponse> => {
    const content = message.content as TranslationEvalContent;

    if (content.translationData) {
      const { confidence, sourceLang, targetLang } = content.translationData;

      // Quality checks
      if (confidence < 0.7) {
        return {
          isValid: false,
          reason: "Low confidence translation",
          suggestions: ["Request human review", "Provide more context"],
        };
      }

      // Language compatibility check
      if (sourceLang === targetLang) {
        return {
          isValid: false,
          reason: "Source and target languages are the same",
        };
      }

      return {
        isValid: true,
        reason: "Translation validated",
        suggestions:
          confidence < 0.9 ? ["Consider reviewing technical terms"] : [],
      };
    }

    return {
      isValid: true,
      reason: "Translation text provided",
      suggestions: ["Add language detection for better accuracy"],
    };
  },

  examples: [
    {
      context: "{{user1}} requests translation",
      messages: [
        {
          user: "{{user1}}",
          content: {
            text: "Translate 'Hello world' to Spanish",
            action: "TRANSLATE",
          },
        },
        {
          user: "TranslateBot",
          content: {
            text: "'Hello world' in Spanish is 'Hola mundo'",
            translationData: {
              sourceText: "Hello world",
              targetText: "Hola mundo",
              sourceLang: "en",
              targetLang: "es",
              confidence: 0.95,
            },
          },
        },
      ],
      outcome: `{
                "isValid": true,
                "reason": "Translation validated"
            }`,
    },
    {
      context: "{{user1}} receives low confidence translation",
      messages: [
        {
          user: "TranslateBot",
          content: {
            text: "Technical translation attempted",
            translationData: {
              sourceText: "Quantum entanglement occurs...",
              targetText: "El entrelazamiento cuántico ocurre...",
              sourceLang: "en",
              targetLang: "es",
              confidence: 0.65,
            },
          },
        },
      ],
      outcome: `{
                "isValid": false,
                "reason": "Low confidence translation",
                "suggestions": ["Request human review", "Provide more context"]
            }`,
    },
  ],

  alwaysRun: true,
};

} // namespace elizaos
