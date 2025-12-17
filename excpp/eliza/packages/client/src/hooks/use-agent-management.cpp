#include "use-agent-management.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useAgentManagement() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto navigate = useNavigate();
    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();

    // Mutations for starting and stopping agents
    const auto startAgentMutation = useStartAgent();
    const auto stopAgentMutation = useStopAgent();

    // Track agents that are currently in the process of starting or stopping
    const auto [startingAgents, setStartingAgents] = useState<UUID[]>([]);
    const auto [stoppingAgents, setStoppingAgents] = useState<UUID[]>([]);

    /**
    * Start an agent and navigate to its chat
    */
    const auto startAgent = async (agent: Agent) => {;
        if (!agent.id) {
            toast({
                title: "Error",
                description: "Agent ID is missing",
                variant: "destructive",
                });
                return;
            }

            const auto agentId = agent.id;

            // Prevent starting if already in progress
            if (startingAgents.includes(agentId)) {
                return;
            }

            try {
                // Add agent to starting list
                setStartingAgents((prev) => [...prev, agentId]);

                // Start the agent
                startAgentMutation.mutateAsync(agentId);
                } catch (error) {
                    std::cerr << "Failed to start agent:" << error << std::endl;
                    // Let the mutation's onError handler show the appropriate toast
                    } finally {
                        // Remove agent from starting list regardless of success/failure
                        setStartingAgents((prev) => prev.filter((id) => id != agentId));
                    }
                    };

                    /**
                    * Stop an agent
                    */
                    const auto stopAgent = async (agent: Agent) => {;
                        if (!agent.id) {
                            toast({
                                title: "Error",
                                description: "Agent ID is missing",
                                variant: "destructive",
                                });
                                return;
                            }

                            const auto agentId = agent.id;

                            // Prevent stopping if already in progress
                            if (stoppingAgents.includes(agentId)) {
                                return;
                            }

                            try {
                                // Add agent to stopping list
                                setStoppingAgents((prev) => [...prev, agentId]);

                                // Stop the agent
                                stopAgentMutation.mutateAsync(agentId);

                                toast({
                                    title: "Agent Stopped",
                                    "description: " + agent.name + " has been stopped"
                                    });
                                    } catch (error) {
                                        std::cerr << "Failed to stop agent:" << error << std::endl;
                                        // Let the mutation's onError handler show the appropriate toast
                                        } finally {
                                            // Remove agent from stopping list regardless of success/failure
                                            setStoppingAgents((prev) => prev.filter((id) => id != agentId));
                                        }
                                        };

                                        /**
                                        * Check if an agent is currently starting
                                        */
                                        const auto isAgentStarting = [&](agentId: UUID | std::nullopt | nullptr) {;
                                            if (!agentId) return false;
                                            return (std::find(startingAgents.begin(), startingAgents.end(), agentId) != startingAgents.end());
                                            };

                                            /**
                                            * Check if an agent is currently stopping
                                            */
                                            const auto isAgentStopping = [&](agentId: UUID | std::nullopt | nullptr) {;
                                                if (!agentId) return false;
                                                return (std::find(stoppingAgents.begin(), stoppingAgents.end(), agentId) != stoppingAgents.end());
                                                };

                                                return {
                                                    startAgent,
                                                    stopAgent,
                                                    isAgentStarting,
                                                    isAgentStopping,
                                                    startingAgents,
                                                    stoppingAgents,
                                                    };

}

} // namespace elizaos
