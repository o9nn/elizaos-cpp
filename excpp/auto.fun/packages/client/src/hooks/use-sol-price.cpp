#include "use-sol-price.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useSolPrice() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return useQuery({;
        queryKey: ["solPrice"],
        queryFn: fetchSolPrice,
        refetchInterval: 60000, // Refresh every minute
        staleTime: 30000, // Consider data stale after 30 seconds
        });

}

} // namespace elizaos
