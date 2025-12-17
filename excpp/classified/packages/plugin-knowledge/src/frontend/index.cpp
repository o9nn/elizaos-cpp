#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void KnowledgeRoute() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = window.ELIZA_CONFIG;
    const auto agentId = config.agentId;

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

        return <KnowledgeProvider agentId={agentId} />;

}

void KnowledgeProvider() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <QueryClientProvider client={queryClient}>;
    <KnowledgeTab agentId={agentId} />;
    </QueryClientProvider>;
    );

}

} // namespace elizaos
