#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "AIService.js.hpp"
#include "TypeScriptParser.js.hpp"
#include "utils/CodeFormatter.js.hpp"
#include "utils/DocumentOrganizer.js.hpp"
#include "utils/prompts.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Interface representing a Frequently Asked Question (FAQ) with a question and its corresponding answer.
 * @typedef {Object} FAQ
 * @property {string} question - The question being asked.
 * @property {string} answer - The answer to the question.
 */
struct FAQ {
    std::string question;
    std::string answer;
};

/**
 * Interface representing a troubleshooting issue.
 * @typedef {object} TroubleshootingIssue
 * @property {string} issue - The description of the issue.
 * @property {string} cause - The cause of the issue.
 * @property {string} solution - The solution to the issue.
 */
struct TroubleshootingIssue {
    std::string issue;
    std::string cause;
    std::string solution;
};

/**
 * Interface representing Troubleshooting information.
 * @interface
 */
struct Troubleshooting {
    std::vector<TroubleshootingIssue> commonIssues;
    std::vector<std::string> debuggingTips;
};

/**
 * Class representing a FullDocumentationGenerator.
 * @class
 */

class FullDocumentationGenerator {
public:
    FullDocumentationGenerator();
    std::future<PluginDocumentation> generatePluginDocumentation(auto packageJson, auto todoItems, auto envUsages);
    std::string formatOverviewSection(const std::any& overview);
    std::string formatFAQSection(const std::vector<FAQ>& faq);
    std::string formatTroubleshootingSection(Troubleshooting troubleshooting);
    std::future<std::string> generateOverview(OrganizedDocs docs, const std::any& packageJson);
    std::string cleanJSONResponse(const std::string& response);
    std::future<std::string> generateInstallation(const std::any& packageJson);
    std::future<std::string> generateBasicInstallPrompt(const std::any& packageJson);
    std::future<std::string> generateConfiguration(const std::vector<EnvUsage>& envUsages);
    std::future<std::string> generateUsage(OrganizedDocs docs, const std::any& _packageJson);
    std::future<std::string> generateApiReference(OrganizedDocs docs);
    std::future<std::string> generateTroubleshooting(OrganizedDocs docs, const std::any& packageJson);
    std::future<std::string> generateFileUsageDoc(FileDocsGroup fileGroup);
    std::future<std::string> generateFileApiDoc(FileDocsGroup fileGroup);
    std::future<std::string> generateProviderDoc(const std::any& provider);
    std::future<TodoSection> generateTodoSection(const std::vector<TodoItem>& todoItems);
    std::string resolveTypeScriptFilePath(const std::string& file);
    std::future<std::string> generateActionsDocumentation(const std::vector<std::string>& actionsFiles);
    std::future<std::string> generateProvidersDocumentation(const std::vector<std::string>& providersFiles);
    std::future<std::string> generateEvaluatorsDocumentation(const std::vector<std::string>& evaluatorsFiles);

private:
    TypeScriptParser typeScriptParser_;
    CodeFormatter codeFormatter_;
    DocumentOrganizer documentOrganizer_;
    AIService aiService_;
};


} // namespace elizaos
