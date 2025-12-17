#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Defines a type representing various AST node types in a JavaScript abstract syntax tree.
 * @typedef {Object} AST_NODE_TYPES
 * @property {string} ClassDeclaration - Represents a class declaration node.
 * @property {string} FunctionDeclaration - Represents a function declaration node.
 * @property {string} TSTypeAliasDeclaration - Represents a TypeScript type alias declaration node.
 * @property {string} TSEnumDeclaration - Represents a TypeScript enum declaration node.
 * @property {string} MethodDefinition - Represents a method definition node.
 * @property {string} TSMethodSignature - Represents a TypeScript method signature node.
 * @property {string} TSInterfaceDeclaration - Represents a TypeScript interface declaration node.
 * @property {string} TSPropertySignature - Represents a TypeScript property signature node.
 * @property {string} ExportNamedDeclaration - Represents an named declaration node.
 * @property {string} Identifier - Represents an identifier node.
 * @property {string} VariableDeclaration - Represents a variable declaration node.
 */

using AST_NODE_TYPES = {

/**
 * Constant object representing AST node types.
 * @constant
 * @readonly
 * @type {Object}
 * @property {string} ClassDeclaration - Represents a class declaration.
 * @property {string} FunctionDeclaration - Represents a function declaration.
 * @property {string} TSTypeAliasDeclaration - Represents a type alias declaration in TypeScript.
 * @property {string} TSEnumDeclaration - Represents an enum declaration in TypeScript.
 * @property {string} MethodDefinition - Represents a method definition.
 * @property {string} TSMethodSignature - Represents a method signature in TypeScript.
 * @property {string} TSInterfaceDeclaration - Represents an interface declaration in TypeScript.
 * @property {string} TSPropertySignature - Represents a property signature in TypeScript.
 * @property {string} ExportNamedDeclaration - Represents an named declaration.
 * @property {string} Identifier - Represents an identifier.
 * @property {string} VariableDeclaration - Represents a variable declaration.
 */

/**
 * Represents types of nodes that are documentable.
 * @typedef {("ClassDeclaration" | "FunctionDeclaration" | "TSTypeAliasDeclaration" | "TSEnumDeclaration" | "MethodDefinition" | "TSMethodSignature" | "TSInterfaceDeclaration" | "TSPropertySignature" | "VariableDeclaration")} DocumentableNodeType
 */
using DocumentableNodeType = std::variant<, 'ClassDeclaration', 'FunctionDeclaration', 'TSTypeAliasDeclaration', 'TSEnumDeclaration', 'MethodDefinition', 'TSMethodSignature', 'TSInterfaceDeclaration', 'TSPropertySignature', 'VariableDeclaration'>;

/**
 * Represents a location within a data structure, specified by a start and end index.
 * @typedef {Object} Location
 * @property {number} start - The starting index of the location.
 * @property {number} end - The ending index of the location.
 */
struct Location {
    double start;
    double end;
};

/**
 * Class to analyze JSDoc comments in TypeScript code.
 */
/**
 * Class representing a JsDocAnalyzer.
 * @property {Set<DocumentableNodeType>} documentableTypes - Set containing various documentable node types.
 */
class JsDocAnalyzer {
public:
    JsDocAnalyzer(TypeScriptParser public typeScriptParser);
    node is TSESTree::VariableDeclaration isVariableDeclaration(TSESTree::Node node);
    bool isConstDeclaration(TSESTree::Node node);
    bool isLongEnough(TSESTree::Node node, auto minLines = 10);
    node is TSESTree::ExportNamedDeclaration isExportDeclaration(TSESTree::Node node);
    bool isSignificantConstant(TSESTree::VariableDeclaration node);
    node is TSESTree::ClassDeclaration isClassDeclaration(TSESTree::Node node);
    node is TSESTree::TSInterfaceDeclaration isInterfaceDeclaration(TSESTree::Node node);
    node is TSESTree::MethodDefinition isMethodDefinition(TSESTree::Node node);
    node is TSESTree::TSMethodSignature isMethodSignature(TSESTree::Node node);
    node is TSESTree::TSPropertySignature isPropertySignature(TSESTree::Node node);
    node is TSESTree::ExportNamedDeclaration isExportNamedDeclaration(TSESTree::Node node);
    node is TSESTree::Identifier isIdentifier(TSESTree::Node node);
    TSESTree::Node getActualNode(TSESTree::Node node);
    std::string getMethodName(TSESTree::MethodDefinition node);
    std::string getNodeName(TSESTree::Node node);
    void analyze(TSESTree::Program ast);
    void traverse(TSESTree::Node node, std::optional<std::vector<TSESTree::Comment>> comments);
    bool shouldHaveJSDoc(TSESTree::Node node);
    std::vector<TSESTree::Node> getDocumentableChildren(TSESTree::Node node);
    ASTQueueItem createQueueItem(TSESTree::Node node, const std::string& filePath, const std::string& code);
    std::string getParentClassName(TSESTree::Node node);
    std::string getParentInterfaceName(TSESTree::Node node);
    bool isClassNode(TSESTree::Node node);
    std::string getJSDocComment(TSESTree::Node node, const std::vector<TSESTree::Comment>& comments);
    Location getNodeLocation(TSESTree::Node node);
    std::vector<TSESTree::MethodDefinition> getClassMethods(const std::string& filePath, std::optional<std::string> className);
    void findTodoComments(TSESTree::Program ast, const std::vector<TSESTree::Comment>& comments, const std::string& sourceCode);
    TSESTree::Node findContainingBlock(TSESTree::Node node);
    void findEnvUsages(TSESTree::Program ast, const std::string& sourceCode);
    std::string extractNodeCode(const std::string& sourceCode, TSESTree::Node node);
    std::string extractFullContext(const std::string& sourceCode, TSESTree::Node node);
    TSESTree::Node findParentStatement(TSESTree::Node node);
    TSESTree::Node findNearestNode(TSESTree::Program ast, double lineNumber);
};


} // namespace elizaos
