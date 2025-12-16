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


using ModelTypeName = std::variant<(typeof ModelType)[keyof typeof ModelType], std::string>;

/**
 * Defines the recognized types of models that the agent runtime can use.
 * These include models for text generation (small, large, reasoning, completion),
 * text embedding, tokenization (encode/decode), image generation and description,
 * audio transcription, text-to-speech, and generic object generation.
 * This constant is used throughout the system, particularly in `AgentRuntime.useModel`,
 * `AgentRuntime.registerModel`, and in `ModelParamsMap` / `ModelResultMap` to ensure
 * type safety and clarity when working with different AI models.
 * String values are used for extensibility with custom model types.
 */

/**
 * Parameters for generating text using a language model.
 * This structure is typically passed to `AgentRuntime.useModel` when the `modelType` is one of
 * `ModelType.TEXT_SMALL`, `ModelType.TEXT_LARGE`, `ModelType.TEXT_REASONING_SMALL`,
 * `ModelType.TEXT_REASONING_LARGE`, or `ModelType.TEXT_COMPLETION`.
 * It includes essential information like the prompt, model type, and various generation controls.
 */
using GenerateTextParams = {
  /** The `AgentRuntime` instance, providing access to models and other services:: */
  /** The input string or prompt that the language model will use to generate text. */
  /** Specifies the type of text generation model to use (e.g., TEXT_LARGE, REASONING_SMALL). */
  /** Optional. The maximum number of tokens to generate in the response. */
  /** Optional. Controls randomness (0.0-1.0). Lower values are more deterministic, higher are more creative. */
  /** Optional. Penalizes new tokens based on their existing frequency in the text so far. */
  /** Optional. Penalizes new tokens based on whether they appear in the text so far. */
  /** Optional. A list of sequences at which the model will stop generating further tokens. */

/**
 * Parameters for detokenizing text, i.e., converting a sequence of numerical tokens back into a string.
 * This is the reverse operation of tokenization.
 * This structure is used with `AgentRuntime.useModel` when the `modelType` is `ModelType.TEXT_TOKENIZER_DECODE`.
 */
struct DetokenizeTextParams {
    std::vector<double> tokens;
    ModelTypeName modelType;
};

/**
 * Base parameters common to all model types
 */
struct BaseModelParams {
    IAgentRuntime runtime;
};

/**
 * Parameters for text generation models
 */
  /** The prompt to generate text from */
  /** Model temperature (0.0 to 1.0, lower is more deterministic) */
  /** Maximum number of tokens to generate */
  /** Sequences that should stop generation when encountered */
  /** Frequency penalty to apply */
  /** Presence penalty to apply */

/**
 * Parameters for text embedding models
 */
  /** The text to create embeddings for */

/**
 * Parameters for text tokenization models
 */
  /** The text to tokenize */
  /** The model type to use for tokenization */

/**
 * Parameters for image generation models
 */
  /** The prompt describing the image to generate */
  /** The dimensions of the image to generate */
  /** Number of images to generate */

/**
 * Parameters for image description models
 */
  /** The URL or path of the image to describe */
  /** Optional prompt to guide the description */

/**
 * Parameters for transcription models
 */
  /** The URL or path of the audio file to transcribe */
  /** Optional prompt to guide transcription */

/**
 * Parameters for text-to-speech models
 */
  /** The text to convert to speech */
  /** The voice to use */
  /** The speaking speed */

/**
 * Parameters for audio processing models
 */
  /** The URL or path of the audio file to process */
  /** The type of audio processing to perform */

/**
 * Parameters for video processing models
 */
  /** The URL or path of the video file to process */
  /** The type of video processing to perform */

/**
 * Optional JSON schema for validating generated objects
 */
using JSONSchema = {

/**
 * Parameters for object generation models
 * @template T - The expected return type, inferred from schema if provided
 */
  /** The prompt describing the object to generate */
  /** Optional JSON schema for validation */
  /** Type of object to generate */
  /** For enum type, the allowed values */
  /** Model type to use */
  /** Model temperature (0.0 to 1.0) */
  /** Sequences that should stop generation */

/**
 * Map of model types to their parameter types
 */
struct ModelParamsMap {
};

/**
 * Map of model types to their return value types
 */
struct ModelResultMap {
};

/**
 * Defines the structure for a model handler registration within the `AgentRuntime`.
 * Each model (e.g., for text generation, embedding) is associated with a handler function,
 * the name of the provider (plugin or system) that registered it, and an optional priority.
 * The `priority` (higher is more preferred) helps in selecting which handler to use if multiple
 * handlers are registered for the same model type. The `registrationOrder` (not in type, but used in runtime)
 * serves as a tie-breaker. See `AgentRuntime.registerModel` and `AgentRuntime.getModel`.
 */
struct ModelHandler {
    std::string provider;
    std::optional<double> priority;
    std::optional<double> registrationOrder;
};


} // namespace elizaos
