#include "actions/cryptoNews.hpp"
#include "actions/webFetch.hpp"
#include "actions/webSearch.hpp"
#include "services/coindeskService.hpp"
#include "services/firecrawlService.hpp"
#include "services/tavilyService.hpp"
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
;
;
;

const webSearchPlugin = {
    name: "webSearch",
    description: "Web search, crypto news, and webpage scraping via Tavily, CoinDesk, and Firecrawl",
    actions: [webSearch, webFetch, cryptoNews],
    evaluators: [],
    providers: [],
    services: [TavilyService, FirecrawlService, CoinDeskService],
    clients: [],
    adapters: [],
};

default webSearchPlugin;

} // namespace elizaos
