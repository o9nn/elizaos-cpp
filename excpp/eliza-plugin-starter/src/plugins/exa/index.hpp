#include "..common/types.ts.hpp"
#include "..common/utils.ts.hpp"
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
;
;

struct ExaSearchResponse {
    Array<{ results;
    std::string title;
    std::string url;
    std::string snippet;
    double score;
};


interface ExaPluginConfig extends SearchPluginConfig {
  searchType?: "semantic" | "code" | "document";
  filters?: {
    language?: string;
    fileType?: string;
  };
}

const DEFAULT_CONFIG: Partial<ExaPluginConfig> = {
  maxResults: 5,
  searchType: "semantic",
};

class ExaSearchPlugin implements SearchPlugin {
  name = "exa-search";
  description = "Search using Exa API for semantic, code, and document search";
  config: ExaPluginConfig;
  private rateLimiter = createRateLimiter(60, 60000); // 60 requests per minute

  constructor(config: ExaPluginConfig) {
    this.config = { ...DEFAULT_CONFIG, ...config };
    validateApiKey(this.config);
  }

  actions: SearchAction[] = [
    {
      name: "EXA_SEARCH",
      description: "Search using Exa API",
      examples: [
        [
          {
            user: "user",
            content: { text: "Find code examples for implementing OAuth" },
          },
        ],
        [
          {
            user: "user",
            content: { text: "Search for documentation about GraphQL" },
          },
        ],
      ],
      similes: ["exa", "exasearch", "semantic web search"],
      validate: async (
        runtime: IAgentRuntime,
        message: Memory,
        state?: State,
      ) => {
        try {
          validateSearchQuery(message.content);
          return true;
        } catch {
          return false;
        }
      },
      handler: async (
        runtime: IAgentRuntime,
        message: Memory,
        state?: State,
      ) => {
        try {
          if (!this.rateLimiter.checkLimit()) {
            return {
              success: false,
              response: "Rate limit exceeded. Please try again later.",
            };
          }

          const query = validateSearchQuery(message.content);
          const response = await fetch("https://api.exa.ai/search", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
              Authorization: `Bearer ${this.config.apiKey}`,
            },
            body: JSON.stringify({
              query,
              search_type: this.config.searchType,
              max_results: this.config.maxResults,
              filters: this.config.filters,
            }),
          });

          if (!response.ok) {
            throw new Error(`Exa API error: ${response.statusText}`);
          }

          const data: ExaSearchResponse = await response.json();
          const results: SearchResult[] = data.results.map((result) => ({
            title: result.title,
            url: result.url,
            snippet: result.snippet,
            score: result.score,
            source: "exa",
          }));

          return {
            success: true,
            response: formatSearchResults(results),
          };
        } catch (error) {
          return handleApiError(error);
        }
      },
    },
  ];
}

default new ExaSearchPlugin({
  apiKey: process.env.EXA_API_KEY || "",
});

} // namespace elizaos
