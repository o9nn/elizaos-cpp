#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "AIService/generators/FullDocumentationGenerator.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Generates comprehensive plugin documentation based on existing JSDoc comments
 */
/**
 * Class for generating comprehensive plugin documentation
 */

class PluginDocumentationGenerator {
public:
    PluginDocumentationGenerator(AIService private aiService, GitManager private gitManager, Configuration private configuration);
    std::future<void> generate(const std::vector<ASTQueueItem>& existingDocs, std::optional<std::string> branchName, const std::vector<TodoItem[] =>& todoItems, const std::vector<EnvUsage[] =>& envUsages);
    std::string generateMarkdownContent(PluginDocumentation docs, const std::any& packageJson);

private:
    FullDocumentationGenerator fullDocumentationGenerator_;
};


} // namespace elizaos
