#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * A class that generates JSDoc comments for code snippets and classes.
 */
/**
 * Constructor for a class that takes in an AIService instance.
 * @param { AIService } aiService - The AIService instance to be injected into the class.
 */
/**
 * Generates a comment based on the given ASTQueueItem.
 * * @param { ASTQueueItem } queueItem - The ASTQueueItem object to generate comment for.
 * @returns {Promise<string>} The generated comment.
 */
/**
 * Generates a comment for a class based on the given ASTQueueItem.
 * * @param { ASTQueueItem } queueItem - The ASTQueueItem to generate the comment for.
 * @returns {Promise<string>} The generated comment for the class.
 */
/**
 * Builds a prompt with the JSDoc comment for the provided ASTQueueItem code.
 * * @param { ASTQueueItem } queueItem The ASTQueueItem object containing the code to extract the JSDoc comment from.
 * @returns { string } The JSDoc comment extracted from the code provided in the ASTQueueItem object.
 */
class JsDocGenerator {
public:
    JsDocGenerator(AIService public aiService);
    std::future<std::string> generateComment(ASTQueueItem queueItem);
    std::future<std::string> generateClassComment(ASTQueueItem queueItem);
    std::string buildPrompt(ASTQueueItem queueItem);
    std::string buildClassPrompt(ASTQueueItem queueItem);
};


} // namespace elizaos
