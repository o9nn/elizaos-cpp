#include "selection.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<std::string>> getLocalAvailableDatabases() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Hard-coded list of available databases to avoid GitHub API calls
    return ["pglite", "postgres"];

}

std::vector<AIModelOption> getAvailableAIModels() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return [;
    {
        title: "Local AI",
        value: "local",
        description: "Local models, no API required",
        },
        {
            title: "OpenAI",
            value: "openai",
            description: "GPT-4 models",
            },
            {
                title: "Anthropic",
                value: "claude",
                description: "Claude models",
                },
                {
                    title: "OpenRouter",
                    value: "openrouter",
                    description: "Access multiple AI models",
                    },
                    {
                        title: "Ollama",
                        value: "ollama",
                        description: "Self-hosted models",
                        },
                        {
                            title: "Google Generative AI",
                            value: "google",
                            description: "Gemini models",
                            },
                            ];

}

std::vector<DatabaseOption> getAvailableDatabases() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return [;
    {
        title: "Pglite (Pglite)",
        value: "pglite",
        description: "Local development",
        },
        {
            title: "PostgreSQL",
            value: "postgres",
            description: "Production database",
            },
            ];

}

std::future<std::string> selectDatabase() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto availableDatabases = getAvailableDatabases();

    const auto database = clack.select({;
        message: "Which database would you like to use?",
        options: availableDatabases.map((db) => ({
            label: db.title,
            value: db.value,
            hint: db.description,
            })),
            initialValue: "pglite",
            });

            if (clack.isCancel(database)) {
                clack.cancel("Operation cancelled.");
                process.exit(0);
            }

            return database;

}

std::future<std::string> selectAIModel() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto availableModels = getAvailableAIModels();

    const auto aiModel = clack.select({;
        message: "Which AI model would you like to use?",
        options: availableModels.map((model) => ({
            label: model.title,
            value: model.value,
            hint: model.description,
            })),
            initialValue: "local",
            });

            if (clack.isCancel(aiModel)) {
                clack.cancel("Operation cancelled.");
                process.exit(0);
            }

            return aiModel;

}

std::vector<AIModelOption> getAvailableEmbeddingModels() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return [;
    {
        title: "Local AI",
        value: "local",
        description: "Local embeddings, no API required",
        },
        {
            title: "OpenAI",
            value: "openai",
            description: "OpenAI text-embedding-ada-002",
            },
            {
                title: "Ollama",
                value: "ollama",
                description: "Self-hosted embedding models",
                },
                {
                    title: "Google Generative AI",
                    value: "google",
                    description: "Google embedding models",
                    },
                    ];

}

std::future<std::string> selectEmbeddingModel() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto availableModels = getAvailableEmbeddingModels();

    const auto embeddingModel = clack.select({;
        message: "Select an embedding model (required since your AI model doesn't support embeddings):",
        options: availableModels.map((model) => ({
            label: model.title,
            value: model.value,
            hint: model.description,
            })),
            initialValue: "local",
            });

            if (clack.isCancel(embeddingModel)) {
                clack.cancel("Operation cancelled.");
                process.exit(0);
            }

            return embeddingModel;

}

} // namespace elizaos
