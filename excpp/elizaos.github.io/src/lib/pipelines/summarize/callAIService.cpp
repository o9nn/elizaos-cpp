#include "callAIService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> callAIService(const std::string& prompt, AISummaryConfig config, std::optional<std::any> options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto model = options.model || config.defaultModel || "openai/gpt-4o-mini";
        std::cout << "Using model:" << model << std::endl;
        try {
            const auto response = fetch(config.endpoint, {;
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                    "Authorization: " + "Bearer " + config.apiKey
                    "HTTP-Referer": process.env.SITE_URL || "https://elizaos.github.io",
                    "X-Title": process.env.SITE_NAME || "ElizaOS Leaderboard",
                    },
                    body: JSON.stringify({
                        model,
                        messages: [
                        {
                            role: "system",
                            content:
                            "You are writing GitHub activity summaries. Use only the actual contribution data provided. Never add, modify or make up information.",
                            },
                            { role: "user", content: prompt },
                            ],
                            temperature: options.temperature || config.temperature,
                            max_tokens: options.maxTokens || config.max_tokens,
                            }),
                            });

                            if (!response.ok) {
                                const auto error = response.text();
                                throw std::runtime_error(`API request failed: ${error}`);
                            }

                            const auto data = response.json();
                            return data.choices[0].message.content.trim();
                            } catch (error) {
                                std::cerr << "Error calling AI service:" << error << std::endl;
                                throw;
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
