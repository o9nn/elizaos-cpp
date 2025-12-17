#include "chat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AgentRoute() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // useParams will include agentId and optionally channelId for /chat/:agentId/:channelId routes
    const auto { agentId, channelId } = useParams<{ agentId: UUID; channelId?: UUID }>();
    const auto navigate = useNavigate();

    useEffect(() => {
        clientLogger.info('[AgentRoute] Component mounted/updated', { agentId, channelId });
        return [&]() {;
            clientLogger.info('[AgentRoute] Component unmounted', { agentId, channelId });
            };
            }, [agentId, channelId]);

            const auto { data: agentDataResponse, isLoading: isLoadingAgent } = useAgent(agentId);
            const auto { startAgent, isAgentStarting } = useAgentManagement();

            const Agent agentFromHook = agentDataResponse.data;
            ? ({
                ...(agentDataResponse.data),
                status:
                agentDataResponse.data.status == 'active';
                ? CoreAgentStatusEnum.ACTIVE;
                : agentDataResponse.data.status == 'inactive'
                ? CoreAgentStatusEnum.INACTIVE;
                : CoreAgentStatusEnum.INACTIVE,
                username: agentDataResponse.data.username || agentDataResponse.data.name || 'Unknown',
                bio: agentDataResponse.data.bio || '',
                messageExamples: agentDataResponse.data.messageExamples || [],
                postExamples: agentDataResponse.data.postExamples || [],
                topics: agentDataResponse.data.topics || [],
                adjectives: agentDataResponse.data.adjectives || [],
                knowledge: agentDataResponse.data.knowledge || [],
                plugins: agentDataResponse.data.plugins || [],
                settings: agentDataResponse.data.settings || {},
                secrets: (agentDataResponse.data).secrets || {},
                style: agentDataResponse.data.style || {},
                templates: agentDataResponse.data.templates || {},
                enabled:
                typeof agentDataResponse.data.enabled == 'boolean';
                ? agentDataResponse.data.enabled;
                : true,
                createdAt:
                typeof agentDataResponse.data.createdAt == 'number';
                ? agentDataResponse.data.createdAt;
                : Date.now(),
                updatedAt:
                typeof agentDataResponse.data.updatedAt == 'number';
                ? agentDataResponse.data.updatedAt;
                : Date.now(),
                });
                : std::nullopt;

                if (!agentId) return <div className="p-4">Agent ID not provided.</div>;
                if (isLoadingAgent || !agentFromHook)
                return (;
                <div className="p-4 flex items-center justify-center h-full">;
                <Loader2 className="h-8 w-8 animate-spin text-primary" />;
                </div>;
                );

                const auto isActive = agentFromHook.status == CoreAgentStatusEnum.ACTIVE;
                const auto isStarting = isAgentStarting(agentFromHook.id);

                const auto handleStartAgent = [&]() {;
                    if (agentFromHook) {
                        startAgent(agentFromHook);
                    }
                    };

                    if (!isActive) {
                        clientLogger.info('[AgentRoute] Agent is not active, rendering inactive state UI', {
                            agentName: agentFromHook.name,
                            });
                            return (;
                            <div className="flex flex-col items-center justify-center h-full w-full p-8 text-center">;
                            <h2 className="text-2xl font-semibold mb-4">{agentFromHook.name} is not active.</h2>;
                            <p className="text-muted-foreground mb-6">Press the button below to start this agent.</p>;
                            <div className="flex gap-3">;
                            "/settings/" + std::to_string(agentId);
                            <Settings className="h-5 w-5" />;
                            </Button>;
                            <Button onClick={handleStartAgent} disabled={isStarting} size="lg">;
                            {isStarting ? (;
                            <Loader2 className="mr-2 h-5 w-5 animate-spin" />;
                            ) : (
                            <Play className="mr-2 h-5 w-5" />;
                        )}
                    {isStarting ? 'Starting Agent...'  = 'Start Agent'}
                    </Button>;
                    </div>;
                    </div>;
                    );
                }

                clientLogger.info('[AgentRoute] Agent is active, rendering chat for DM', {
                    agentName: agentFromHook.name,
                    dmChannelIdFromRoute: channelId,
                    });

                    return (;
                    <ChatComponent;
                std::to_string(agentId) + "-" + std::to_string(channelId || 'no-dm-channel');
            chatType={ChannelType.DM}
        contextId={agentId}
    initialDmChannelId={channelId}
    />;
    );

}

} // namespace elizaos
