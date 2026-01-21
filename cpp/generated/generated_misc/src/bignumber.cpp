#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/utils/bignumber.h"

std::string formatBN(std::shared_ptr<BigNumber> value, double decimals)
{
    try
    {
        return value->dividedBy(((std::make_shared<BigNumber>(10)))->pow(decimals))->toFixed();
    }
    catch (const std::any& error)
    {
        throw std::any(std::make_shared<Error>(std::string("Failed to format BigNumber: ") + value + string_empty));
    }
};



void Main(void)
{
    BigNumber->config(object{
        object::pair{std::string("DECIMAL_PLACES"), 18}, 
        object::pair{std::string("ROUNDING_MODE"), BigNumber->ROUND_DOWN}, 
        object::pair{std::string("EXPONENTIAL_AT"), array<any>{ -20, 20 }}
    });
}

MAIN
