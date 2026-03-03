#include "plugin.hpp"
#include <iostream>

namespace elizaos {

// Plugin initialization and registration
// In a full ElizaOS runtime, this would register the providers with the agent runtime.
// For now, we provide the static classes that can be called directly.

/**
 * Example usage:
 *
 *   // DreamGen cloud generation
 *   std::string result = DreamGenProvider::generate(
 *       "Tell me about the ancient forest",
 *       "You are Eliza, a wise and empathetic AI companion.",
 *       api_key,
 *       DreamGenProvider::MODEL_LARGE,
 *       500, 0.8,
 *       "Eliza"  // character name triggers text-role mode
 *   );
 *
 *   // KoboldCpp local generation
 *   std::string result = KoboldCppProvider::generate(
 *       "What is the meaning of life?",
 *       "You are a helpful assistant.",
 *       "http://localhost:5001",
 *       500, 0.7
 *   );
 *
 *   // KoboldCpp native API with advanced sampling
 *   std::string result = KoboldCppProvider::generate_native(
 *       "<|system|>You are helpful.<|user|>Hello!<|assistant|>",
 *       "http://localhost:5001",
 *       500, 0.7, 1.1, 40, 0.9
 *   );
 */

} // namespace elizaos
