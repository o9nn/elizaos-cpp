#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-coinmarketcap/src/actions/getPrice/examples.h"

array<array<std::shared_ptr<ActionExample>>> priceExamples = array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
    object::pair{std::string("user"), std::string("{{user1}}")}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("What's the current price of Bitcoin?")}
    }}
}, object{
    object::pair{std::string("user"), std::string("{{agent}}")}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("Let me check the current Bitcoin price for you.")}, 
        object::pair{std::string("action"), std::string("GET_PRICE")}
    }}
}, object{
    object::pair{std::string("user"), std::string("{{agent}}")}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("The current price of BTC is 65,432.21 USD")}
    }}
} }, array<object>{ object{
    object::pair{std::string("user"), std::string("{{user1}}")}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("Check ETH price in EUR")}
    }}
}, object{
    object::pair{std::string("user"), std::string("{{agent}}")}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("I'll check the current Ethereum price in EUR.")}, 
        object::pair{std::string("action"), std::string("GET_PRICE")}
    }}
}, object{
    object::pair{std::string("user"), std::string("{{agent}}")}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("The current price of ETH is 2,345.67 EUR")}
    }}
} } };

void Main(void)
{
}

MAIN
