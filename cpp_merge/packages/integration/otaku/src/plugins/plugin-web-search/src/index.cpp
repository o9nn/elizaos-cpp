#include "index.hpp"

object webSearchPlugin = object{
    object::pair{std:("name"), std:("webSearch")}, 
    object::pair{std:("description"), std:("Web search, crypto news, and webpage scraping via Tavily, CoinDesk, and Firecrawl")}, 
    object::pair{std:("actions"), array<any>{ webSearch, webFetch, cryptoNews }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("services"), array<TavilyService>{ TavilyService, FirecrawlService, CoinDeskService }}, 
    object::pair{std:("clients"), array<any>()}, 
    object::pair{std:("adapters"), array<any>()}
};

void Main(void)
{
}

MAIN
