#include "index.hpp"
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

/**
 * Example usage of the SWE-agent rules module
 */

// Example 1: Validate a Python file
std::future<void> validatePythonExample();

// Example 2: Validate a TypeScript file
std::future<void> validateTypeScriptExample();

// Example 3: Get project structure information
void projectStructureExample();

// Example 4: Get applicable rules for different file types
void rulesExample();

// Example 5: Export rules to Cursor IDE format
void exportExample();

// Run all examples
std::future<void> runExamples();

// Execute if run directly


} // namespace elizaos
