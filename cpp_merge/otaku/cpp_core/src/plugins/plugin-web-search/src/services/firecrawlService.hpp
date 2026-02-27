#include "elizaos/core.hpp"
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



struct FirecrawlScrapeOptions {
    std::optional<std::vector<'markdown' | 'html' | 'rawHtml' | 'screenshot' | 'links'>> formats;
    std::optional<bool> onlyMainContent;
    std::optional<std::vector<std::string>> includeTags;
    std::optional<std::vector<std::string>> excludeTags;
    std::optional<double> waitFor;
    std::optional<double> timeout;
};

struct FirecrawlScrapeResponse {
    bool success;
    std::optional<{> data;
    std::optional<std:> markdown;
    std::optional<std:> html;
    std::optional<std:> rawHtml;
    std::optional<std:> screenshot;
    std::optional<std::vector<std::string>> links;
    std::optional<{> metadata;
    std::optional<std:> title;
    std::optional<std:> description;
    std::optional<std:> language;
    std::optional<std:> ogTitle;
    std::optional<std:> ogDescription;
    std::optional<std:> ogUrl;
    std::optional<std:> ogImage;
    std::optional<std:> error;
};

class FirecrawlService extends Service {
    static serviceType = "FIRECRAWL" as const;
    capabilityDescription = "Fetch and scrape webpage content using Firecrawl API";
    
    private apiKey: std:;
    private baseUrl: std: = "https://api.firecrawl.dev/v1";

    static std::async start(runtime: IAgentRuntime): Promise<FirecrawlService> {
        const service = std::make_unique<FirecrawlService>();
        service.initialize(runtime);
        return service;
    }

    /**
     * Scrape a URL using Firecrawl API
     * @param url - The URL to scrape
     * @param options - Optional scraping configuration
     * @returns Promise with scraped content
     */

    /**
     * Check if the service is properly configured
     */

    /**
     * Stop the service (cleanup if needed)
     */
        // No cleanup needed for Firecrawl service

} // namespace elizaos
