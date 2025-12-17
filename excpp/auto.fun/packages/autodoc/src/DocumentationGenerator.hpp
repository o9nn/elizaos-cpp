#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "JSDocValidator.js.hpp"
#include "PluginDocumentationGenerator.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Class representing a Documentation Generator.
 *
 */
/**
 * Class representing a Documentation Generator.
 * @property {ASTQueueItem[]} missingJsDocQueue - Array representing missing JSDoc queue.
 * @property {ASTQueueItem[]} existingJsDocQueue - Array representing existing JSDoc queue.
 * @property { boolean } hasChanges - Flag indicating if there are changes.
 * @property {Map<string, string>} fileContents - Map containing file contents.
 * @property { string } branchName - Name of the branch.
 * @property {Map<string, number>} fileOffsets - Map containing file offsets.
 * @property {string[]} typeScriptFiles - Array containing TypeScript files.
 * @property { JSDocValidator } jsDocValidator - Instance of JSDocValidator.
 */
class DocumentationGenerator {
public:
    DocumentationGenerator();
    Promise< generate(std::optional<double> pullNumber);
    void processNode(TSESTree::Node node, const std::string& filePath, TSESTree::Program ast);
    std::future<void> updateFileWithJSDoc(const std::string& filePath, const std::string& jsDoc, double insertLine);
    std::string getNodeCode(const std::string& filePath, TSESTree::Node node);
    std::future<std::string> getFileContent(const std::string& contentsUrl);
    Promise< generatePRContent(std::optional<double> pullNumber);
    std::string generateDefaultPRBody();
    Promise< analyzeCodebase();

private:
    JSDocValidator jsDocValidator_;
};


} // namespace elizaos
