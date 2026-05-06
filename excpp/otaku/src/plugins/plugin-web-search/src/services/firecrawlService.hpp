#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct FirecrawlScrapeOptions {
    std::optional<std::variant<Array<'markdown', std::string, 'links'>>> formats;
    std::optional<bool> onlyMainContent;
    std::optional<std::vector<std::string>> includeTags;
    std::optional<std::vector<std::string>> excludeTags;
    std::optional<double> waitFor;
    std::optional<double> timeout;
};

struct FirecrawlScrapeResponse {
    bool success;
    std::optional<std::string> markdown;
    std::optional<std::string> html;
    std::optional<std::string> rawHtml;
    std::optional<std::string> screenshot;
    std::optional<std::vector<std::string>> links;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::string> language;
    std::optional<std::string> ogTitle;
    std::optional<std::string> ogDescription;
    std::optional<std::string> ogUrl;
    std::optional<std::string> ogImage;
    std::optional<std::string> error;
};

class FirecrawlService {
public:
    static std::future<FirecrawlService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<FirecrawlScrapeResponse> scrape(const std::string& url, FirecrawlScrapeOptions options = {});
    bool isConfigured();
    std::future<void> stop();


} // namespace elizaos
