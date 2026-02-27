#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-coinmarketcap/src/actions/getPrice/examples.h"

array<array<std::shared_ptr<ActionExample>>> priceExamples = array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
    object::pair{std:("user"), std:("{{user1}}")}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("What's the current price of Bitcoin?")}
    }}
}, object{
    object::pair{std:("user"), std:("{{agent}}")}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("Let me check the current Bitcoin price for you.")}, 
        object::pair{std:("action"), std:("GET_PRICE")}
    }}
}, object{
    object::pair{std:("user"), std:("{{agent}}")}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("The current price of BTC is 65,432.21 USD")}
    }}
} }, array<object>{ object{
    object::pair{std:("user"), std:("{{user1}}")}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("Check ETH price in EUR")}
    }}
}, object{
    object::pair{std:("user"), std:("{{agent}}")}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("I'll check the current Ethereum price in EUR.")}, 
        object::pair{std:("action"), std:("GET_PRICE")}
    }}
}, object{
    object::pair{std:("user"), std:("{{agent}}")}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("The current price of ETH is 2,345.67 EUR")}
    }}
} } };

void Main(void)
{
}

MAIN
