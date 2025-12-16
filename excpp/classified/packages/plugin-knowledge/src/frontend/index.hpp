#include "ui/knowledge-tab.tsx.hpp"
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
;
import type { UUID } from '@elizaos/core';

const queryClient = new QueryClient();

// Define the interface for the ELIZA_CONFIG
struct ElizaConfig {
    std::string agentId;
    std::string apiBase;
};


// Declare global window extension for TypeScript
declare global {
  struct Window {
    std::optional<ElizaConfig> ELIZA_CONFIG;
};

}

/**
 * Main Knowledge route component
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

  return <KnowledgeProvider agentId={agentId as UUID} />;
}

/**
 * Knowledge provider component
 */
: { agentId: UUID }) {
  return (
    <QueryClientProvider client={queryClient}>
      <KnowledgeTab agentId={agentId} />
    </QueryClientProvider>
  );
}

// Initialize the application - no router needed for iframe
const rootElement = document.getElementById('root');
if (rootElement) {
  createRoot(rootElement).render(<KnowledgeRoute />);
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


struct KnowledgePanelProps {
    std::string agentId;
};


/**
 * Knowledge panel component for the plugin system
 */
const KnowledgePanelComponent: React.FC<KnowledgePanelProps> = ({ agentId }) => {
  return <KnowledgeTab agentId={agentId as UUID} />;
};

// Export the panel configuration for integration with the agent UI
const panels: AgentPanel[] = [
  {
    name: 'Knowledge',
    path: 'knowledge',
    component: KnowledgePanelComponent,
    icon: 'Book',
    public: false,
    shortLabel: 'Know',
  },
];

* from './utils';

} // namespace elizaos
