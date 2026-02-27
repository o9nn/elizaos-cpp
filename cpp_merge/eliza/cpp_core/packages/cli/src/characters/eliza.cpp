#include "eliza.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

Character getElizaCharacter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto plugins = [;
    // Core plugins first
    "@elizaos/plugin-sql",

    // Text-only plugins (no embedding support)
    ...(std::getenv("ANTHROPIC_API_KEY") ? ["@elizaos/plugin-anthropic"] : []),
    ...(std::getenv("OPENROUTER_API_KEY") ? ["@elizaos/plugin-openrouter"] : []),

    // Embedding-capable plugins last (lowest priority for embedding fallback)
    ...(std::getenv("OPENAI_API_KEY") ? ["@elizaos/plugin-openai"] : []),
    ...(std::getenv("OLLAMA_API_ENDPOINT") ? ["@elizaos/plugin-ollama"] : []),
    ...(std::getenv("GOOGLE_GENERATIVE_AI_API_KEY") ? ["@elizaos/plugin-google-genai"] : []),
    ...(!std::getenv("GOOGLE_GENERATIVE_AI_API_KEY") &&;
    !std::getenv("OLLAMA_API_ENDPOINT") &&;
    !std::getenv("OPENAI_API_KEY");
    ? ["@elizaos/plugin-local-ai"];
    : []),

    // Platform plugins
    ...(std::getenv("DISCORD_API_TOKEN") ? ["@elizaos/plugin-discord"] : []),
    ...(std::getenv("TWITTER_API_KEY") &&;
    std::getenv("TWITTER_API_SECRET_KEY") &&;
    std::getenv("TWITTER_ACCESS_TOKEN") &&;
    std::getenv("TWITTER_ACCESS_TOKEN_SECRET");
    ? ["@elizaos/plugin-twitter"];
    : []),
    ...(std::getenv("TELEGRAM_BOT_TOKEN") ? ["@elizaos/plugin-telegram"] : []),

    // Bootstrap plugin
    ...(!std::getenv("IGNORE_BOOTSTRAP") ? ["@elizaos/plugin-bootstrap"] : []),
    ];

    return {
        ...baseCharacter,
        plugins,
        }

}

} // namespace elizaos
