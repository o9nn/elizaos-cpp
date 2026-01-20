#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "AIService/AIService.js.hpp"
#include "Configuration.js.hpp"
#include "DirectoryTraversal.js.hpp"
#include "DocumentationGenerator.js.hpp"
#include "GitManager.js.hpp"
#include "JsDocAnalyzer.js.hpp"
#include "JsDocGenerator.js.hpp"
#include "PluginDocumentationGenerator.js.hpp"
#include "TypeScriptParser.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Main function for generating documentation.
 * Uses configuration initialized from the GitHub workflow file.
 * @async
 */
/**
 * Asynchronous function that serves as the main entry point for generating documentation.
 *
 * This function:
 * 1. Initializes necessary components like Configuration, GitManager, DirectoryTraversal,
 *    TypeScriptParser, JsDocAnalyzer, AIService, JsDocGenerator, DocumentationGenerator,
 *    PluginDocumentationGenerator, etc.
 * 2. Analyzes the codebase and generates JSDoc documentation.
 * 3. Conditionally updates README and creates a pull request for documentation changes.
 *
 * @returns {Promise<void>} A promise that resolves when the documentation generation is complete.
 */

std::future<void> main();

// Simple error handling for the main function

} // namespace elizaos
