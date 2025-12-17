#include "providers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Providers() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [queryClient] = useState(() => new QueryClient({;
        defaultOptions: {
            queries: {
                staleTime: 30 * 1000,
                refetchInterval: 30 * 1000,
                },
                },
                }));

                return (;
                <PHProvider>;
                <QueryClientProvider client={queryClient}>;
            {children}
            <ReactQueryDevtools initialIsOpen={false} />;
            </QueryClientProvider>;
            </PHProvider>;
            );

}

} // namespace elizaos
