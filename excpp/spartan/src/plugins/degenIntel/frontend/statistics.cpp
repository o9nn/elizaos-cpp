#include "statistics.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Statistics() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto query = useQuery({;
        queryKey: ["statistics"],
        queryFn: async () => {
            const auto response = "fetch(" + import.meta.env.VITE_API_URL + "/statistics";
                method: "POST",
                });
                const auto data = response.json();
                return data;
                },
                refetchInterval: 5_000,
                });

                return (;
                <div className="py-4 w-full bg-muted">;
                <div className="container flex items-center gap-4">;
                {query.isPending ? (;
                <div className="text-sm animate-pulse">Loading</div>;
                ) : (
                <div className="flex items-center gap-4 text-sm">;
                <span>📚 Tweets {query.data.tweets}</span>;
                <span className="text-muted">•</span>;
                <span>🌍 Sentiment {query.data.sentiment}</span>;
                <span>•</span>;
                <span>💸 Tokens {query.data.tokens}</span>;
                <span>•</span>;
                <span>⛓️ Chains 3</span>;
                </div>;
            )}
            </div>;
            </div>;
            );

}

} // namespace elizaos
