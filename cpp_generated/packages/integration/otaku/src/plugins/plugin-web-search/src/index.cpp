#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-web-search/src/index.h"

object webSearchPlugin = object{
    object::pair{std::string("name"), std::string("webSearch")}, 
    object::pair{std::string("description"), std::string("Web search, crypto news, and webpage scraping via Tavily, CoinDesk, and Firecrawl")}, 
    object::pair{std::string("actions"), array<any>{ webSearch, webFetch, cryptoNews }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("services"), array<TavilyService>{ TavilyService, FirecrawlService, CoinDeskService }}, 
    object::pair{std::string("clients"), array<any>()}, 
    object::pair{std::string("adapters"), array<any>()}
};

void Main(void)
{
}

MAIN
