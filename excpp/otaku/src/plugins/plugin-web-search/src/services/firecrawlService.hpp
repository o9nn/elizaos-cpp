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

;

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
    std::optional<std::string> markdown;
    std::optional<std::string> html;
    std::optional<std::string> rawHtml;
    std::optional<std::string> screenshot;
    std::optional<std::vector<std::string>> links;
    std::optional<{> metadata;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::string> language;
    std::optional<std::string> ogTitle;
    std::optional<std::string> ogDescription;
    std::optional<std::string> ogUrl;
    std::optional<std::string> ogImage;
    std::optional<std::string> error;
};


class FirecrawlService extends Service {
    static serviceType = "FIRECRAWL" as const;
    capabilityDescription = "Fetch and scrape webpage content using Firecrawl API";
    
    private apiKey: string;
    private baseUrl: string = "https://api.firecrawl.dev/v1";

    static async start(runtime: IAgentRuntime): Promise<FirecrawlService> {
        const service = new FirecrawlService();
        await service.initialize(runtime);
        return service;
    }

    async initialize(runtime: IAgentRuntime): Promise<void> {
        this.apiKey = runtime.getSetting("FIRECRAWL_API_KEY") ?? "";
        
        if (!this.apiKey) {
            logger.warn("FIRECRAWL_API_KEY not configured. WEB_FETCH action will not be available.");
        } else {
            logger.info("FirecrawlService initialized successfully");
        }
    }

    /**
     * Scrape a URL using Firecrawl API
     * @param url - The URL to scrape
     * @param options - Optional scraping configuration
     * @returns Promise with scraped content
     */
    async scrape(
        url: string,
        options: FirecrawlScrapeOptions = {}
    ): Promise<FirecrawlScrapeResponse> {
        if (!this.apiKey) {
            throw new Error("FIRECRAWL_API_KEY is not configured");
        }

        try {
            logger.info(`[Firecrawl] Scraping URL: ${url}`);

            const requestBody = {
                url,
                formats: options.formats || ['markdown', 'html'],
                onlyMainContent: options.onlyMainContent ?? true,
                ...(options.includeTags && { includeTags: options.includeTags }),
                ...(options.excludeTags && { excludeTags: options.excludeTags }),
                ...(options.waitFor && { waitFor: options.waitFor }),
                ...(options.timeout && { timeout: options.timeout }),
            };

            const response = await fetch(`${this.baseUrl}/scrape`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${this.apiKey}`,
                },
                body: JSON.stringify(requestBody),
            });

            if (!response.ok) {
                const errorText = await response.text();
                logger.error(`[Firecrawl] API error (${response.status}): ${errorText}`);
                return {
                    success: false,
                    error: `Firecrawl API error: ${response.status} - ${errorText}`,
                };
            }

            const result = await response.json();
            
            logger.info(`[Firecrawl] Successfully scraped ${url}`);
            
            return {
                success: true,
                data: result.data,
            };
        } catch (error) {
            const errorMsg = error instanceof Error ? error.message : String(error);
            logger.error(`[Firecrawl] Scrape failed for ${url}: ${errorMsg}`);
            return {
                success: false,
                error: errorMsg,
            };
        }
    }

    /**
     * Check if the service is properly configured
     */
    isConfigured(): boolean {
        return !!this.apiKey;
    }

    /**
     * Stop the service (cleanup if needed)
     */
    async stop(): Promise<void> {
        // No cleanup needed for Firecrawl service
        logger.info("FirecrawlService stopped");
    }
}

} // namespace elizaos
