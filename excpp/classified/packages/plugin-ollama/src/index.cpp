#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string messagesToPrompt(const std::vector<std::any>& messages) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!Array.isArray(messages)) {
        return '';
    }

    return messages;
    .map((msg) => {
        if (typeof msg == 'string') {
            return msg;
        }

        // Handle standard message format
        if (msg.role && msg.content) {
            const auto role =;
            msg.role == 'user' ? 'User' : msg.role == 'assistant' ? 'Assistant' : msg.role;

            // Handle content that's an array of parts
            if (Array.isArray(msg.content)) {
                const auto textParts = msg.content;
                .filter((part: any) => part.type == 'text')
                .map((part: any) => part.text)
                .join(' ');
                return std::to_string(role) + ": " + std::to_string(textParts);
            }

            // Handle content that's a string
            if (typeof msg.content == 'string') {
                return std::to_string(role) + ": " + std::to_string(msg.content);
            }
        }

        return '';
        });
        .filter(Boolean);
        .join('\n');

}

std::future<void> generateOllamaText(ReturnType<typeof createOllama> ollama, const std::string& model, std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { text: ollamaResponse } = generateText({;
            model: ollama(model),
            prompt: params.prompt,
            system: params.system,
            temperature: params.temperature,
            maxTokens: params.maxTokens,
            frequencyPenalty: params.frequencyPenalty,
            presencePenalty: params.presencePenalty,
            stopSequences: params.stopSequences,
            });
            return ollamaResponse;
            } catch (error: unknown) {
                std::cerr << 'Error in generateOllamaText:' << error << std::endl;
                return 'Error generating text. Please try again later.';
            }

}

std::future<void> generateOllamaObject(ReturnType<typeof createOllama> ollama, const std::string& model, ObjectGenerationParams params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { object } = generateObject({;
            model: ollama(model),
            output: 'no-schema',
            prompt: params.prompt,
            temperature: params.temperature,
            });
            return object;
            } catch (error: unknown) {
                std::cerr << 'Error generating object:' << error << std::endl;
                return {}
            }

}

} // namespace elizaos
