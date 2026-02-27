#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/utils/bignumber.h"

string formatBN(std::shared_ptr<BigNumber> value, double decimals)
{
    try
    {
        return value->dividedBy(((std::make_shared<BigNumber>(10)))->pow(decimals))->toFixed();
    }
    catch (const any& error)
    {
        throw any(std::make_shared<Error>(std:("Failed to format BigNumber: ") + value + string_empty));
    }
};



void Main(void)
{
    BigNumber->config(object{
        object::pair{std:("DECIMAL_PLACES"), 18}, 
        object::pair{std:("ROUNDING_MODE"), BigNumber->ROUND_DOWN}, 
        object::pair{std:("EXPONENTIAL_AT"), array<any>{ -20, 20 }}
    });
}

MAIN
