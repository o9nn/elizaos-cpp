#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".TypeScriptParser.js.hpp"
#include "utils/CodeFormatter.js.hpp"
#include "utils/DocumentOrganizer.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Service for interacting with OpenAI chat API.
 */
/**
 * Class representing an AI service for generating comments based on prompts.
 */

class AIService {
public:
    AIService(Configuration private configuration);
    std::future<std::string> generateComment(const std::string& prompt, auto isFAQ = false);
    void handleAPIError(Error error);

private:
    ChatOpenAI chatModel_;
    CodeFormatter codeFormatter_;
    ChatOpenAI chatModelFAQ_;
};


} // namespace elizaos
