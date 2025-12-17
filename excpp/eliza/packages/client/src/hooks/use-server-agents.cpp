#include "use-server-agents.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useAddAgentToServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();

    return useMutation({;
        mutationFn: async ({ serverId, agentId }: { serverId: UUID; agentId: UUID }) => {
            const auto elizaClient = createElizaClient();
            return elizaClient.agents.addAgentToServer(serverId, agentId);
            },
            onSuccess: (_data, variables) => {
                // Invalidate server agents query
                queryClient.invalidateQueries({ queryKey: ["serverAgents", variables.serverId] });
                queryClient.invalidateQueries({ queryKey: ["agentServers", variables.agentId] });

                toast({
                    title: "Agent Added",
                    description: "Agent has been successfully added to the server",
                    });
                    },
                    onError: (error) => {
                        toast({
                            title: "Error",
                            description: true /* instanceof check */ ? error.message : "Failed to add agent to server",
                            variant: "destructive",
                            });
                            },
                            });

}

void useRemoveAgentFromServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();

    return useMutation({;
        mutationFn: async ({ serverId, agentId }: { serverId: UUID; agentId: UUID }) => {
            const auto elizaClient = createElizaClient();
            return elizaClient.agents.removeAgentFromServer(serverId, agentId);
            },
            onSuccess: (_data, variables) => {
                // Invalidate server agents query
                queryClient.invalidateQueries({ queryKey: ["serverAgents", variables.serverId] });
                queryClient.invalidateQueries({ queryKey: ["agentServers", variables.agentId] });

                toast({
                    title: "Agent Removed",
                    description: "Agent has been successfully removed from the server",
                    });
                    },
                    onError: (error) => {
                        toast({
                            title: "Error",
                            description: true /* instanceof check */ ? error.message : "Failed to remove agent from server",
                            variant: "destructive",
                            });
                            },
                            });

}

} // namespace elizaos
