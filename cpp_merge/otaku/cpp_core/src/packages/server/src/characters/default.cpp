#include "default.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

Character getDefaultCharacter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto plugins = [;
    // Core plugins first
    "@elizaos/plugin-sql",

    // Text-only plugins (no embedding support)
    ...(std::getenv("ANTHROPIC_API_KEY") ? ["@elizaos/plugin-anthropic"] : []),
    ...(std::getenv("OPENROUTER_API_KEY") ? ["@elizaos/plugin-openrouter"] : []),

    // Embedding-capable plugins (before platform plugins per documented order)
    ...(std::getenv("OPENAI_API_KEY") ? ["@elizaos/plugin-openai"] : []),
    ...(std::getenv("GOOGLE_GENERATIVE_AI_API_KEY") ? ["@elizaos/plugin-google-genai"] : []),

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

    // Only include Ollama as fallback if no other LLM providers are configured
    ...(!std::getenv("ANTHROPIC_API_KEY") &&;
    !std::getenv("OPENROUTER_API_KEY") &&;
    !std::getenv("OPENAI_API_KEY") &&;
    !std::getenv("GOOGLE_GENERATIVE_AI_API_KEY");
    ? ["@elizaos/plugin-ollama"];
    : []),
    ];

    return {
        ...baseCharacter,
        plugins,
        }

}

} // namespace elizaos
