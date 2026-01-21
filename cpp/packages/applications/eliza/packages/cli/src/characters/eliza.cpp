#include "eliza.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Character getElizaCharacter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto plugins = [;
    // Core plugins first
    "@elizaos/plugin-sql",

    // Text-only plugins (no embedding support)
    ...(process.env.ANTHROPIC_API_KEY ? ["@elizaos/plugin-anthropic"] : []),
    ...(process.env.OPENROUTER_API_KEY ? ["@elizaos/plugin-openrouter"] : []),

    // Embedding-capable plugins last (lowest priority for embedding fallback)
    ...(process.env.OPENAI_API_KEY ? ["@elizaos/plugin-openai"] : []),
    ...(process.env.OLLAMA_API_ENDPOINT ? ["@elizaos/plugin-ollama"] : []),
    ...(process.env.GOOGLE_GENERATIVE_AI_API_KEY ? ["@elizaos/plugin-google-genai"] : []),
    ...(!process.env.GOOGLE_GENERATIVE_AI_API_KEY &&;
    !process.env.OLLAMA_API_ENDPOINT &&;
    !process.env.OPENAI_API_KEY;
    ? ["@elizaos/plugin-local-ai"];
    : []),

    // Platform plugins
    ...(process.env.DISCORD_API_TOKEN ? ["@elizaos/plugin-discord"] : []),
    ...(process.env.TWITTER_API_KEY &&;
    process.env.TWITTER_API_SECRET_KEY &&;
    process.env.TWITTER_ACCESS_TOKEN &&;
    process.env.TWITTER_ACCESS_TOKEN_SECRET;
    ? ["@elizaos/plugin-twitter"];
    : []),
    ...(process.env.TELEGRAM_BOT_TOKEN ? ["@elizaos/plugin-telegram"] : []),

    // Bootstrap plugin
    ...(!process.env.IGNORE_BOOTSTRAP ? ["@elizaos/plugin-bootstrap"] : []),
    ];

    return {
        ...baseCharacter,
        plugins,
        }

}

} // namespace elizaos
