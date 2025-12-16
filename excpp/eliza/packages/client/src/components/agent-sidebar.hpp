#include "agent-action-viewer.hpp"
#include "agent-log-viewer.hpp"
#include "agent-memory-viewer.hpp"
#include "ui/skeleton.hpp"
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
import type { UUID, Agent } from '@elizaos/core';
;
;
;
;
;
;
;
;

using AgentSidebarProps = std::variant<{
  agentId: UUID, std::nullopt>;
  agentName: string;
  channelId?: UUID;
};

using FixedTabValue = std::variant<'details', 'actions', 'logs', 'memories'>;
using TabValue = std::variant<FixedTabValue, std::string>;

: AgentSidebarProps) {
  const { currentTab: detailsTab, setTab: setDetailsTab } = useAgentTabState(agentId);
  const { data: panelsResponse, isLoading: isLoadingPanels } = useAgentPanels(agentId!, {
    enabled: !!agentId,
  });

  const {
    data: agentDataResponse,
    isLoading: isLoadingAgent,
    error: agentError,
  } = useAgent(agentId, { enabled: !!agentId && detailsTab === 'details' });

  const agent = agentDataResponse?.data as Agent | undefined;

  const agentPanels = useMemo(() => {
    return panelsResponse?.data || [];
  }, [panelsResponse]);

  const allTabs: { value: TabValue; label: string; icon: JSX.Element }[] = useMemo(() => {
    const fixedTabs: { value: FixedTabValue; label: string; icon: JSX.Element }[] = [
      { value: 'details', label: 'Details', icon: <InfoIcon className="h-4 w-4" /> },
      { value: 'actions', label: 'Actions', icon: <Eye className="h-4 w-4" /> },
      { value: 'memories', label: 'Memories', icon: <Database className="h-4 w-4" /> },
      { value: 'logs', label: 'Logs', icon: <Code className="h-4 w-4" /> },
    ];

    const dynamicTabs = agentPanels.map((panel: AgentPanel) => ({
      value: panel.name,
      label: panel.name,
      icon: <Columns3 className="h-4 w-4" />,
    }));

    return [...fixedTabs, ...dynamicTabs];
  }, [agentPanels]);

  return (
    <Tabs
      defaultValue="details"
      value={detailsTab}
      onValueChange={setDetailsTab}
      className="flex flex-col h-screen w-full max-w-full"
    >
      <TabsList className="flex w-full max-w-full overflow-x-auto flex-shrink-0">
        {allTabs.map((tab) => (
          <TabsTrigger
            key={tab.value}
            value={tab.value}
            className="flex items-center gap-1.5 flex-shrink-0"
          >
            {tab.icon}
            <span className="truncate">{tab.label}</span>
          </TabsTrigger>
        ))}
        {isLoadingPanels && (
          <>
            {[...Array(2)].map((_, i) => (
              <Skeleton key={`skel-tab-${i}`} className="h-9 w-full rounded-md flex-shrink-0" />
            ))}
          </>
        )}
      </TabsList>

      <TabsContent
        value="details"
        className="overflow-y-auto overflow-x-hidden flex-1 p-4 w-full max-w-full min-h-0"
      >
        {detailsTab === 'details' && agentId && (
          <>
            {isLoadingAgent && (
              <div className="flex items-center justify-center h-full">
                <Loader2 className="h-6 w-6 animate-spin text-primary" />
              </div>
            )}
            {agentError && (
              <div className="text-red-500 break-words">
                Error loading agent details: {agentError.message}
              </div>
            )}
            {!isLoadingAgent && !agentError && agent && (
              <div className="w-full max-w-full">
                <AgentSettings
                  agent={agent}
                  agentId={agentId}
                  onSaveComplete={() => {
                    // Stay on the same page after save in sidebar context
                    // Agent settings saved from sidebar
                  }}
                />
              </div>
            )}
            {!isLoadingAgent && !agentError && !agent && !isLoadingPanels && (
              <div className="text-muted-foreground">Agent details not found.</div>
            )}
          </>
        )}
        {detailsTab === 'details' && !agentId && (
          <div className="p-4 text-muted-foreground">Select an agent to see their details.</div>
        )}
      </TabsContent>

      <TabsContent
        value="actions"
        className="overflow-y-auto overflow-x-hidden flex-1 w-full max-w-full min-h-0"
      >
        {detailsTab === 'actions' && agentId && (
          <div className="w-full max-w-full">
            <AgentActionViewer agentId={agentId} />
          </div>
        )}
        {detailsTab === 'actions' && !agentId && (
          <div className="p-4 text-muted-foreground">Select an agent to see their actions.</div>
        )}
      </TabsContent>
      <TabsContent
        value="logs"
        className="overflow-y-auto overflow-x-hidden flex-1 w-full max-w-full min-h-0"
      >
        {detailsTab === 'logs' && agentId && (
          <div className="w-full max-w-full">
            <AgentLogViewer agentName={agentName} level="all" />
          </div>
        )}
        {detailsTab === 'logs' && !agentId && (
          <div className="p-4 text-muted-foreground">Select an agent to see their logs.</div>
        )}
      </TabsContent>
      <TabsContent
        value="memories"
        className="overflow-y-auto overflow-x-hidden flex-1 w-full max-w-full min-h-0"
      >
        {detailsTab === 'memories' && agentId && (
          <div className="w-full max-w-full">
            <AgentMemoryViewer agentId={agentId} agentName={agentName} channelId={channelId} />
          </div>
        )}
        {detailsTab === 'memories' && !agentId && (
          <div className="p-4 text-muted-foreground">Select an agent to see their memories.</div>
        )}
      </TabsContent>
      {agentPanels.map((panel: AgentPanel) => (
        <TabsContent
          key={panel.name}
          value={panel.name}
          className="overflow-y-auto overflow-x-hidden flex-1 w-full max-w-full min-h-0"
        >
          {detailsTab === panel.name && agentId && (
            <iframe
              src={panel.url}
              title={panel.name}
              className="w-full h-full border-0 flex-1 max-w-full"
            />
          )}
        </TabsContent>
      ))}
    </Tabs>
  );
}

} // namespace elizaos
