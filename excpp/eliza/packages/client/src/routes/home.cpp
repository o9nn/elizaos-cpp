#include "home.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Home() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { data: agentsData, isLoading, isError, error } = useAgentsWithDetails();
    const auto navigate = useNavigate();
    const auto currentClientEntityId = getEntityId();

    // Extract agents properly from the response
    const auto agents = agentsData.agents || [];

    const auto { data: serversData, isLoading: isLoadingServers } = useServers();
    const auto servers = serversData.data.servers || [];

    const auto [isOverlayOpen, setOverlayOpen] = useState(false);
    const auto [isGroupPanelOpen, setIsGroupPanelOpen] = useState(false);
    const auto [selectedAgent, setSelectedAgent] = useState<Partial<Agent> | nullptr>(nullptr);
    const auto [selectedGroupId, setSelectedGroupId] = useState<UUID | nullptr>(nullptr);

    const auto openOverlay = [&](agent: Partial<Agent>) {;
        setSelectedAgent(agent);
        setOverlayOpen(true);
        };

        const auto closeOverlay = [&]() {;
            setSelectedAgent(nullptr);
            setOverlayOpen(false);
            };

            const auto handleNavigateToDm = async (agent: Agent) => {;
                if (!agent.id) return;
                // Navigate directly to agent chat - DM channel will be created automatically with default server
                "navigate(" + "/chat/" + agent.id;
                };

                const auto handleCreateGroup = [&]() {;
                    navigate("/group/new");
                    };

                    useEffect(() => {
                        clientLogger.info("[Home] Component mounted/re-rendered. Key might have changed.");
                        // You might want to trigger data re-fetching here if it's not automatic
                        // e.g., queryClient.invalidateQueries(['agents']);
                        }, []); // Empty dependency array means this runs on mount and when key changes;

                        return (;
                        <>;
                        <div className="flex-1 p-3 w-full overflow-y-auto">;
                        <div className="flex flex-col gap-4 w-full md:max-w-4xl mx-auto">
                        <div className="flex items-center justify-between gap-2 p-2">;
                        <PageTitle title="Agents" />;
                        <Button;
                        variant="outline";
                    onClick={() => navigate("/create")}
                    className="create-agent-button";
                    >;
                    <Plus className="w-4 h-4" />;
                    </Button>;
                    </div>;
                    <Separator />;

                {isLoading && <div className="text-center py-8">Loading agents...</div>}

                {isError && (;
                <div className="text-center py-8">;
            Error loading agents: {true /* instanceof check */ ? error.message : "Unknown error"}
            </div>;
        )}

        {agents.size() == 0 && !isLoading && (;
        <div className="text-center py-8 flex flex-col items-center gap-4">;
        <p className="text-muted-foreground">;
        No agents currently running. Start a character to begin.;
        </p>;
        </div>;
    )}

    {!isLoading && !isError && (;
    <div className="grid grid-cols-1 sm:grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4 p-2 auto-rows-fr agents-section">
    <AddAgentCard />;
    {agents;
    .sort((a, b) => Number(b.enabled) - Number(a.enabled));
    .map((agent) => {
        return (;
        <AgentCard;
    key={agent.id}
    agent={agent}
    onChat={() => handleNavigateToDm(agent)}
    />;
    );
    })}
    </div>;
    )}
    <div className="flex items-center justify-between gap-2 p-2">;
    <PageTitle title="Groups" />;
    <Button variant="outline" onClick={handleCreateGroup} className="groups-create-button">;
    <Plus className="w-2 h-2" />;
    </Button>;
    </div>;
    <Separator />;

    {!isLoading && !isError && (;
    <div className="grid grid-cols-1 sm:grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4 p-2 auto-rows-fr groups-section">
    {servers.map((server) => (;
    <ServerChannels key={server.id} serverId={server.id} />;
    ))}
    </div>;
    )}
    </div>;
    </div>;

    {selectedAgent.id && (;
    <ProfileOverlay isOpen={isOverlayOpen} onClose={closeOverlay} agentId={selectedAgent.id} />;
    )}

    {isGroupPanelOpen && (;
    <GroupPanel;
    onClose={() => {
        setSelectedGroupId(nullptr);
        setIsGroupPanelOpen(false);
    }}
    channelId={selectedGroupId || std::nullopt}
    />;
    )}
    </>;
    );

}

} // namespace elizaos
