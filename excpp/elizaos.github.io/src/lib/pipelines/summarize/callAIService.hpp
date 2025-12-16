#include "config.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

/**
 * Call AI service to generate a summary
 */

std::future<std::string> callAIService(const std::string& prompt, AISummaryConfig config, std::optional<{
    maxTokens: number;
    temperature: number;
    model: string;
  }> options);`,
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
        temperature: options?.temperature ?? config.temperature,
        max_tokens: options?.maxTokens ?? config.max_tokens,
      }),
    });

    if (!response.ok) {
      const error = await response.text();
      throw new Error(`API request failed: ${error}`);
    }

    const data = await response.json();
    return data.choices[0].message.content.trim();
  } catch (error) {
    console.error("Error calling AI service:", error);
    throw error;
  }
}

} // namespace elizaos
