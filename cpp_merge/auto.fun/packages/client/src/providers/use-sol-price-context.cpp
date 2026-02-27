#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/providers/use-sol-price-context.h"

std::function<any()> useSolPriceContext = [=]() mutable
{
    return useContext(SolPriceContext);
};
any SolPriceContext = createContext<std::shared_ptr<SolPriceContextType>>(object{
    object::pair{std:("solPrice"), undefined}, 
    object::pair{std:("isLoading"), false}, 
    object::pair{std:("error"), nullptr}
});

void Main(void)
{
}

MAIN
