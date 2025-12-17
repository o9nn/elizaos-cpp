#include "default.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Character getDefaultCharacter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto plugins = [;
    // Core plugins first
    '@elizaos/plugin-sql',

    // Text-only plugins (no embedding support)
    ...(process.env.ANTHROPIC_API_KEY.trim() ? ['@elizaos/plugin-anthropic'] : []),
    ...(process.env.OPENROUTER_API_KEY.trim() ? ['@elizaos/plugin-openrouter'] : []),

    // Embedding-capable plugins (before platform plugins per documented order)
    ...(process.env.OPENAI_API_KEY.trim() ? ['@elizaos/plugin-openai'] : []),
    ...(process.env.GOOGLE_GENERATIVE_AI_API_KEY.trim() ? ['@elizaos/plugin-google-genai'] : []),

    // Platform plugins
    ...(process.env.DISCORD_API_TOKEN.trim() ? ['@elizaos/plugin-discord'] : []),
    ...(process.env.TWITTER_API_KEY.trim() &&;
    process.env.TWITTER_API_SECRET_KEY.trim() &&;
    process.env.TWITTER_ACCESS_TOKEN.trim() &&;
    process.env.TWITTER_ACCESS_TOKEN_SECRET.trim();
    ? ['@elizaos/plugin-twitter'];
    : []),
    ...(process.env.TELEGRAM_BOT_TOKEN.trim() ? ['@elizaos/plugin-telegram'] : []),

    // Bootstrap plugin
    ...(!process.env.IGNORE_BOOTSTRAP ? ['@elizaos/plugin-bootstrap'] : []),

    // Only include Ollama as fallback if no other LLM providers are configured
    ...(!process.env.ANTHROPIC_API_KEY.trim() &&;
    !process.env.OPENROUTER_API_KEY.trim() &&;
    !process.env.OPENAI_API_KEY.trim() &&;
    !process.env.GOOGLE_GENERATIVE_AI_API_KEY.trim();
    ? ['@elizaos/plugin-ollama'];
    : []),
    ];

    return {
        ...baseCharacter,
        plugins,
        }

}

} // namespace elizaos
