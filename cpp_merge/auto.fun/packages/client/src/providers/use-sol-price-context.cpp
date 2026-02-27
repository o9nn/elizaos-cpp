#include "use-sol-price-context.h"
#include <string>

std::function<any()> useSolPriceContext = [=]() mutable
{
    return useContext(SolPriceContext);
};
any SolPriceContext = createContext<std::shared_ptr<SolPriceContextType>>(object{
    object::pair{std::string("solPrice"), std::nullopt}, 
    object::pair{std::string("isLoading"), false}, 
    object::pair{std::string("error"), nullptr}
});

void Main(void)
{
}

MAIN
