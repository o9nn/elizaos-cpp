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

struct TavilySearchResponse {
    Array<{ results;
    std::string title;
    std::string url;
    std::string content;
};


interface TavilyPluginConfig extends SearchPluginConfig {
  searchType?: "search" | "news" | "academic";
}

const DEFAULT_CONFIG: Partial<TavilyPluginConfig> = {
  maxResults: 5,
  searchType: "search",
};

class TavilySearchPlugin implements SearchPlugin {
  readonly name: string = "tavily-search";
  readonly description: string = "Search the web using Tavily API";
  config: TavilyPluginConfig;
  private rateLimiter = createRateLimiter(60, 60000); // 60 requests per minute

  constructor(config: TavilyPluginConfig) {
    this.config = { ...DEFAULT_CONFIG, ...config };
    validateApiKey(this.config);
  }

  actions: SearchAction[] = [
    {
      name: "TAVILY_SEARCH",
      description: "Search the web using Tavily API",
      examples: [
        [
          {
            user: "user",
            content: { text: "Search for recent AI developments" },
          },
        ],
        [
          {
            user: "user",
            content: { text: "Find news about climate change" },
          },
        ],
      ],
      similes: ["tavily", "tavilysearch", "agent search"],
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
          const response = await fetch("https://api.tavily.com/search", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
              Authorization: `Bearer ${this.config.apiKey}`,
            },
            body: JSON.stringify({
              query,
              search_type: this.config.searchType,
              max_results: this.config.maxResults,
            }),
          });

          if (!response.ok) {
            throw new Error(`Tavily API error: ${response.statusText}`);
          }

          const data: TavilySearchResponse = await response.json();
          const results: SearchResult[] = data.results.map((result) => ({
            title: result.title,
            url: result.url,
            snippet: result.content,
            source: "tavily",
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

default new TavilySearchPlugin({
  apiKey: process.env.TAVILY_API_KEY || "",
});

} // namespace elizaos
