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
 * Interface representing a group of documentation for files.
 *
 * @property {string} filePath - The file path for the group of documentation.
 * @property {ASTQueueItem[]} classes - An array of ASTQueueItem objects representing classes in the file.
 * @property {ASTQueueItem[]} methods - An array of ASTQueueItem objects representing methods in the file.
 * @property {ASTQueueItem[]} interfaces - An array of ASTQueueItem objects representing interfaces in the file.
 * @property {ASTQueueItem[]} types - An array of ASTQueueItem objects representing types in the file.
 * @property {ASTQueueItem[]} functions - An array of ASTQueueItem objects representing functions in the file.
 */
struct FileDocsGroup {
    std::string filePath;
    std::vector<ASTQueueItem> classes;
    std::vector<ASTQueueItem> methods;
    std::vector<ASTQueueItem> interfaces;
    std::vector<ASTQueueItem> types;
    std::vector<ASTQueueItem> functions;
};

/**
 * Interface representing a collection of organized documentation items.
 * @property {ASTQueueItem[]} classes - An array of ASTQueueItem objects representing classes.
 * @property {ASTQueueItem[]} methods - An array of ASTQueueItem objects representing methods.
 * @property {ASTQueueItem[]} interfaces - An array of ASTQueueItem objects representing interfaces.
 * @property {ASTQueueItem[]} types - An array of ASTQueueItem objects representing types.
 * @property {ASTQueueItem[]} functions - An array of ASTQueueItem objects representing functions.
 * @property {ASTQueueItem[]} variables - An array of ASTQueueItem objects representing variables.
 */
struct OrganizedDocs {
    std::vector<ASTQueueItem> classes;
    std::vector<ASTQueueItem> methods;
    std::vector<ASTQueueItem> interfaces;
    std::vector<ASTQueueItem> types;
    std::vector<ASTQueueItem> functions;
    std::vector<ASTQueueItem> variables;
};


} // namespace elizaos
