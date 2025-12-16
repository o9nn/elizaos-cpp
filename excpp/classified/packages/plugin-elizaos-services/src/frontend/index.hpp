#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
import './index.css';
;
import type { UUID } from '@elizaos/core';

const queryClient = new QueryClient();

// Define the interface for the ELIZA_CONFIG
struct ElizaConfig {
    std::string agentId;
    std::string apiBase;
};


// Define the interface for time response
struct TimeResponse {
    std::string timestamp;
    double unix;
    std::string formatted;
    std::string timezone;
};


/**
 * Time display component that fetches from backend
 */
: { apiBase: string }) {
  const { data, isLoading, error, refetch } = useQuery<TimeResponse>({
    queryKey: ['currentTime'],
    queryFn: async () => {
      const response = await fetch(`${apiBase}/api/time`);
      if (!response.ok) {
        throw new Error('Failed to fetch time');
      }
      return response.json();
    },
    refetchInterval: 1000, // Refresh every second
  });

  if (isLoading) {
    return <div className="text-gray-600">Loading time...</div>;
  }

  if (error) {
    return (
      <div className="text-red-600">
        Error fetching time: {error instanceof Error ? error.message : 'Unknown error'}
      </div>
    );
  }

  return (
    <div className="space-y-2" data-testid="time-display">
      <h2 className="text-lg font-semibold">Current Time</h2>
      <div className="space-y-1 text-sm">
        <div>
          <span className="font-medium">Formatted:</span> {data?.formatted}
        </div>
        <div>
          <span className="font-medium">Timezone:</span> {data?.timezone}
        </div>
        <div>
          <span className="font-medium">Unix:</span> {data?.unix}
        </div>
      </div>
      <button
        onClick={() => refetch()}
        className="px-3 py-1 text-sm bg-primary text-primary-foreground rounded hover:opacity-90"
        data-testid="refresh-button"
      >
        Refresh
      </button>
    </div>
  );
}

/**
 * Main Example route component
 */
, []);

  if (!agentId) {
    return (
      <div className="p-4 text-center">
        <div className="text-red-600 font-medium">Error: Agent ID not found</div>
        <div className="text-sm text-gray-600 mt-2">
          The server should inject the agent ID configuration.
        </div>
      </div>
    );
  }

  return <ExampleProvider agentId={agentId as UUID} apiBase={apiBase} />;
}

/**
 * Example provider component
 */
: { agentId: UUID; apiBase: string }) {
  return (
    <QueryClientProvider client={queryClient}>
      <div className="p-6 space-y-6">
        <div>
          <h1 className="text-2xl font-bold mb-2">Plugin Starter Example</h1>
          <div className="text-sm text-muted-foreground">Agent ID: {agentId}</div>
        </div>
        <TimeDisplay apiBase={apiBase} />
      </div>
    </QueryClientProvider>
  );
}

// Initialize the application - no router needed for iframe
const rootElement = document.getElementById('root');
if (rootElement) {
  createRoot(rootElement).render(<ExampleRoute />);
}

// Define types for integration with agent UI system
struct AgentPanel {
    std::string name;
    std::string path;
    React.ComponentType<any> component;
    std::optional<std::string> icon;
    std::optional<bool> public;
    std::optional<string; // Optional short label for mobile> shortLabel;
};


struct PanelProps {
    std::string agentId;
};


/**
 * Example panel component for the plugin system
 */
const PanelComponent: React.FC<PanelProps> = ({ agentId }) => {
  return (
    <div className="p-4">
      <h2 className="text-lg font-semibold mb-2">Example Panel</h2>
      <div>Hello {agentId}!</div>
    </div>
  );
};

// Export the panel configuration for integration with the agent UI
const panels: AgentPanel[] = [
  {
    name: 'Example',
    path: 'example',
    component: PanelComponent,
    icon: 'Book',
    public: false,
    shortLabel: 'Example',
  },
];

* from './utils';

} // namespace elizaos
