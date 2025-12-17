#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TimeDisplay() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { data, isLoading, error, refetch } = useQuery<TimeResponse>({;
            queryKey: ['currentTime'],
            queryFn: async () => {
                const auto response = std::to_string(apiBase) + "/api/time";
                if (!response.ok) {
                    throw std::runtime_error('Failed to fetch time');
                }
                return response.json();
                },
                refetchInterval: 1000, // Refresh every second
                });

                if (isLoading) {
                    return <div className="text-gray-600">Loading time...</div>;
                }

                if (error) {
                    return (;
                    <div className="text-red-600">;
                Error fetching time: {true /* instanceof check */ ? error.message : 'Unknown error'}
                </div>;
                );
            }

            return (;
            <div className="space-y-2" data-testid="time-display">;
            <h2 className="text-lg font-semibold">Current Time</h2>;
            <div className="space-y-1 text-sm">;
            <div>;
        <span className="font-medium">Formatted:</span> {data.formatted}
        </div>;
        <div>;
        <span className="font-medium">Timezone:</span> {data.timezone}
        </div>;
        <div>;
        <span className="font-medium">Unix:</span> {data.unix}
        </div>;
        </div>;
        <button;
        onClick={() => refetch()}
        className="px-3 py-1 text-sm bg-primary text-primary-foreground rounded hover:opacity-90"
        data-testid="refresh-button";
        >;
        Refresh;
        </button>;
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void ExampleRoute() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = (window).ELIZA_CONFIG | std::nullopt;
    const auto agentId = config.agentId;
    const auto apiBase = config.apiBase || 'http://localhost:3000';

    // Apply dark mode to the root element
    React.useEffect(() => {
        document.documentElement.classList.add('dark');
        }, []);

        if (!agentId) {
            return (;
            <div className="p-4 text-center">;
            <div className="text-red-600 font-medium">Error: Agent ID not found</div>
            <div className="text-sm text-gray-600 mt-2">;
            The server should inject the agent ID configuration.;
            </div>;
            </div>;
            );
        }

        return <ExampleProvider agentId={agentId} apiBase={apiBase} />;

}

void ExampleProvider() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <QueryClientProvider client={queryClient}>;
    <div className="p-6 space-y-6">;
    <div>;
    <h1 className="text-2xl font-bold mb-2">Plugin Starter Example</h1>;
    <div className="text-sm text-muted-foreground">Agent ID: {agentId}</div>
    </div>;
    <TimeDisplay apiBase={apiBase} />;
    </div>;
    </QueryClientProvider>;
    );

}

} // namespace elizaos
