#include "sol-price-provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SolPriceProvider(const std::any& { children }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { data: solPrice, isLoading, error } = useSolPrice();

    return (;
    <SolPriceContext.Provider;
    value={{ solPrice, isLoading, error: error }}
    >;
    {children}
    </SolPriceContext.Provider>;
    );

}

} // namespace elizaos
