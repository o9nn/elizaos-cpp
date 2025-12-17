#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Retrieves the Groq API base URL, using runtime settings or environment variables if available.
 *
 * @returns The resolved Groq API base URL.
 */
std::string getBaseURL(const std::any& runtime);

// Removed unused Runtime interface

/**
 * Returns the appropriate Groq model name string for the specified model type.
 *
 * If environment variables for model names are set, they are used; otherwise, defaults are returned.
 *
 * @param model - The model type for which to retrieve the model name.
 * @returns The model name string corresponding to the given {@link model}.
 *
 * @remark If an error occurs, returns the default model name 'llama-3.1-8b-instant'.
 */
TiktokenModel findModelName(ModelTypeName model);

std::future<void> tokenizeText(ModelTypeName model, const std::string& prompt);

/**
 * Detokenize a sequence of tokens back into text using the specified model.
 *
 * @param {ModelTypeName} model - The type of model to use for detokenization.
 * @param {number[]} tokens - The sequence of tokens to detokenize.
 * @returns {string} The detokenized text.
 */
std::future<void> detokenizeText(ModelTypeName model, const std::vector<double>& tokens);

/**
 * Generate text using Groq API
 */
std::future<void> generateGroqText(ReturnType<typeof createGroq> groq, const std::string& model, std::optional<std::any> params);

/**
 * Generate object using Groq API
 */
std::future<void> generateGroqObject(ReturnType<typeof createGroq> groq, const std::string& model, ObjectGenerationParams params);

      throw Error("Missing GROQ_API_KEY in environment variables");

            // Encode the string into tokens first
            // Now decode tokens back into text

} // namespace elizaos
