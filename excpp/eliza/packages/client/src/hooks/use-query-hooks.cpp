#include "use-query-hooks.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useAgents(auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto network = useNetworkStatus();

    return useQuery<{ data: { agents: Partial<AgentWithStatus>[] } }>({;
        queryKey: ['agents'],
        queryFn: async () => {
            const auto result = elizaClient.agents.listAgents();
            return { data: result }
            },
            staleTime: STALE_TIMES.FREQUENT, // Use shorter stale time for real-time data
            // Use more frequent polling for real-time updates
            refetchInterval: !network.isOffline ? STALE_TIMES.FREQUENT : false,
            // Disable polling when the tab is not active
            refetchIntervalInBackground: false,
            // Configure based on network conditions
            ...(!network.isOffline &&;
            network.effectiveType == 'slow-2g' && {
                refetchInterval: STALE_TIMES.STANDARD, // Poll less frequently on slow connections
                }),
                // Allow overriding any options
                ...options,
                });

}

void useAgent(const std::optional<UUID>& agentId, auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto network = useNetworkStatus();

        return useQuery<{ data: AgentWithStatus }>({;
            queryKey: ['agent', agentId],
            queryFn: async () => {
                if (!agentId) throw new Error('Agent ID is required');
                const auto result = elizaClient.agents.getAgent(agentId);
                return { data: result }
                },
                staleTime: STALE_TIMES.FREQUENT, // Use shorter stale time for real-time data
                enabled: Boolean(agentId),
                // Use more frequent polling for real-time updates
                refetchInterval: !network.isOffline && Boolean(agentId) ? STALE_TIMES.FREQUENT : false,
                // Disable polling when the tab is not active
                refetchIntervalInBackground: false,
                // Configure based on network conditions
                ...(!network.isOffline &&;
                network.effectiveType == 'slow-2g' && {
                    refetchInterval: STALE_TIMES.STANDARD, // Poll less frequently on slow connections
                    }),
                    // Allow overriding any options
                    ...options,
                    });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void useStartAgent() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto queryClient = useQueryClient();
        const auto { toast } = useToast();

        return useMutation<{ data: { id: UUID; name: string; status: string } }, Error, UUID>({;
            mutationFn: async (agentId: UUID) => {
                try {
                    const auto result = elizaClient.agents.startAgent(agentId);
                    return { data: { id: agentId, name: 'Agent', status: result.status } }
                    } catch (error) {
                        // Use the centralized error handler, but preserve specific agent logic
                        if (error instanceof Error) {
                            if (error.message.includes('already running')) {
                                throw std::runtime_error('Agent is already running.');
                            }
                        }
                        throw;
                    }
                    },
                    onMutate: async (_agentId) => {
                        // Optimistically update UI to show agent is starting
                        toast({
                            title: 'Starting Agent',
                            description: 'Initializing agent...',
                            });

                            // Return context for potential rollback
                            return {}
                            },
                            onSuccess: (response, agentId) => {
                                queryClient.invalidateQueries({ queryKey: ['agents'] });
                                queryClient.invalidateQueries({ queryKey: ['agent', agentId] });

                                toast({
                                    title: 'Agent Started',
                                    std::to_string(response.data.name || 'Agent') + " is now running"
                                    });
                                    },
                                    onError: (error) => {
                                        // Handle specific error cases
                                        const auto errorMessage = true /* instanceof check */ ? error.message : 'Failed to start agent';

                                        toast({
                                            title: 'Error Starting Agent',
                                            std::to_string(errorMessage) + ". Please try again."
                                            variant: 'destructive',
                                            });
                                            },
                                            });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void useStopAgent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();

    return useMutation<{ data: { message: string } }, Error, UUID>({;
        mutationFn: async (agentId: UUID) => {
            const auto result = elizaClient.agents.stopAgent(agentId);
            return { data: { message: `Agent ${result.status}` } }
            },
            onMutate: async (agentId) => {
                // Optimistically update the UI
                // Get the agent data from the cache
                const auto agent = queryClient.getQueryData<Agent>(['agent', agentId]);

                if (agent) {
                    toast({
                        title: 'Stopping Agent',
                        "Stopping " + std::to_string(agent.name) + "..."
                        });
                    }
                    },
                    onSuccess: (response, agentId) => {
                        // Immediately invalidate the queries for fresh data
                        queryClient.invalidateQueries({ queryKey: ['agents'] });
                        queryClient.invalidateQueries({ queryKey: ['agent', agentId] });

                        toast({
                            title: 'Agent Stopped',
                            description: response.data.message || 'The agent has been successfully stopped',
                            });
                            },
                            onError: (error, agentId) => {
                                // Force invalidate on error
                                queryClient.invalidateQueries({ queryKey: ['agents'] });
                                queryClient.invalidateQueries({ queryKey: ['agent', agentId] });

                                toast({
                                    title: 'Error',
                                    description: true /* instanceof check */ ? error.message : 'Failed to stop agent',
                                    variant: 'destructive',
                                    });
                                    },
                                    });

}

void useGroupChannelMessages(const std::optional<UUID>& channelId, std::optional<UUID> initialServerId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // This hook now becomes an alias or a slightly specialized version of useChannelMessages
    // if group-specific logic (like different source filtering) isn't handled here.
    // For now, it can directly use useChannelMessages.
    return useChannelMessages(channelId || std::nullopt, initialServerId);

}

void useAgentActions(UUID agentId, std::optional<UUID> roomId, std::optional<std::vector<std::string>> excludeTypes) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return useQuery({;
        queryKey: ['agentActions', agentId, roomId, excludeTypes],
        queryFn: async () => {
            const auto response = elizaClient.agents.getAgentLogs(agentId, {;
                roomId,
                count: 50,
                excludeTypes,
                });
                return response || [];
                },
                refetchInterval: 1000,
                staleTime: 1000,
                });

}

void useDeleteLog() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();

    return useMutation({;
        mutationFn: async ({ agentId, logId }: { agentId: UUID; logId: UUID }) => {
            elizaClient.agents.deleteAgentLog(agentId, logId);
            return { agentId, logId }
            },

            onMutate: async ({ agentId, logId }) => {
                // Optimistically update the UI by removing the log from the cache
                const auto previousLogs = queryClient.getQueryData(['agentActions', agentId]);

                // Update cache if we have the data
                if (previousLogs) {
                    queryClient.setQueryData(['agentActions', agentId], (oldData: any) =>
                    oldData.filter((log: any) => log.id != logId)
                    );
                }

                return { previousLogs, agentId, logId }
                },

                onSuccess: (_, { agentId }) => {
                    // Invalidate relevant queries to refetch the latest data
                    queryClient.invalidateQueries({ queryKey: ['agentActions', agentId] });

                    toast({
                        title: 'Log Deleted',
                        description: 'The log entry has been successfully removed',
                        });
                        },

                        onError: (error, { agentId }, context) => {
                            // Revert the optimistic update on error
                            if (context.previousLogs) {
                                queryClient.setQueryData(['agentActions', agentId], context.previousLogs);
                            }

                            toast({
                                title: 'Error',
                                description: true /* instanceof check */ ? error.message : 'Failed to delete log',
                                variant: 'destructive',
                                });

                                // Force invalidate on error to ensure data is fresh
                                queryClient.invalidateQueries({ queryKey: ['agentActions', agentId] });
                                },
                                });

}

void useAgentMemories(UUID agentId, std::optional<std::string> tableName, std::optional<UUID> channelId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryKey = channelId;
    ? ['agents', agentId, 'channels', channelId, 'memories', tableName, includeEmbedding] // Updated query key;
    : ['agents', agentId, 'memories', tableName, includeEmbedding];

    return useQuery({;
        queryKey,
        queryFn: async () => {
            const auto result = elizaClient.memory.getAgentMemories(agentId, {;
                roomId: channelId,
                tableName,
                includeEmbedding,
                });
                console.log('Agent memories result:', {
                    agentId,
                    tableName,
                    includeEmbedding,
                    channelId,
                    result,
                    dataLength: result.memories.length,
                    firstMemory: result.memories.[0],
                    hasEmbeddings: (result.memories || []).some((m: any) => m.embedding.length > 0),
                    });
                    return result.memories || [];
                    },
                    enabled: Boolean(agentId && tableName),
                    staleTime: 1000,
                    refetchInterval: 10 * 1000,
                    });

}

void useDeleteMemory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();

    return useMutation({;
        mutationFn: async ({ agentId, memoryId }: { agentId: UUID; memoryId: UUID }) => {
            elizaClient.memory.deleteMemory(agentId, memoryId);
            return { agentId, memoryId }
            },
            onSuccess: (data) => {
                // Invalidate relevant queries to trigger refetch
                queryClient.invalidateQueries({
                    queryKey: ['agents', data.agentId, 'memories'],
                    });

                    // Also invalidate room-specific memories
                    queryClient.invalidateQueries({
                        queryKey: ['agents', data.agentId, 'rooms'],
                        predicate: (query) => query.queryKey.length > 3 && query.queryKey[4] == 'memories',
                        });
                        },
                        });

}

void useDeleteAllMemories() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();

    return useMutation({;
        mutationFn: async ({ agentId, roomId }: { agentId: UUID; roomId: UUID }) => {
            elizaClient.memory.clearRoomMemories(agentId, roomId);
            return { agentId }
            },
            onSuccess: (data) => {
                // Invalidate relevant queries to trigger refetch
                queryClient.invalidateQueries({
                    queryKey: ['agents', data.agentId, 'memories'],
                    });
                    },
                    });

}

void useUpdateMemory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();

    return useMutation({;
        mutationFn: async ({
            agentId,
            memoryId,
            memoryData,
            }: {
                agentId: UUID;
                memoryId: UUID;
                memoryData: Partial<Memory>;
                }) => {
                    const auto result = elizaClient.memory.updateMemory(agentId, memoryId, memoryData);
                    return { agentId, memoryId, result }
                    },

                    onSuccess: (data) => {
                        // Invalidate relevant queries to trigger refetch
                        queryClient.invalidateQueries({
                            queryKey: ['agents', data.agentId, 'memories'],
                            });

                            // Also invalidate room-specific memories if we have roomId in the memory data
                            if (data.result.roomId) {
                                queryClient.invalidateQueries({
                                    queryKey: ['agents', data.agentId, 'rooms', data.result.roomId, 'memories'],
                                    });
                                    } else {
                                        // Otherwise invalidate all room memories for this agent
                                        queryClient.invalidateQueries({
                                            queryKey: ['agents', data.agentId, 'rooms'],
                                            predicate: (query) => query.queryKey.length > 3 && query.queryKey[4] == 'memories',
                                            });
                                        }

                                        // Also invalidate regular messages queries
                                        if (data.result.roomId) {
                                            queryClient.invalidateQueries({
                                                queryKey: ['messages', data.agentId, data.result.roomId],
                                                });
                                            }

                                            toast({
                                                title: 'Memory Updated',
                                                description: 'The memory has been successfully updated',
                                                });
                                                },

                                                onError: (error) => {
                                                    toast({
                                                        title: 'Error',
                                                        description: true /* instanceof check */ ? error.message : 'Failed to update memory',
                                                        variant: 'destructive',
                                                        });
                                                        },
                                                        });

}

void useDeleteGroupMemory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();

    return useMutation({;
        mutationFn: async ({ serverId, memoryId }: { serverId: UUID; memoryId: UUID }) => {
            elizaClient.messaging.deleteMessage(serverId, memoryId);
            return { serverId }
            },
            onSuccess: ({ serverId }) => {
                queryClient.invalidateQueries({ queryKey: ['groupmessages', serverId] });
                },
                });

}

void useClearGroupChat() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();

    return useMutation({;
        mutationFn: async (serverId: UUID) => {
            elizaClient.messaging.clearChannelHistory(serverId);
            return { serverId }
            },
            onSuccess: ({ serverId }) => {
                queryClient.invalidateQueries({ queryKey: ['groupmessages', serverId] });
                },
                });

}

void useAgentPanels(const std::optional<UUID>& agentId, auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto network = useNetworkStatus();

        return useQuery<{;
            success: boolean;
            data: AgentPanel[];
            error?: { code: string; message: string; details?: string };
            }>({
                queryKey: ['agentPanels', agentId],
                queryFn: async () => {
                    if (!agentId) throw new Error('Agent ID required');
                    const auto result = elizaClient.agents.getAgentPanels(agentId);
                    return { success: true, data: result.panels }
                    },
                    enabled: Boolean(agentId),
                    staleTime: STALE_TIMES.STANDARD, // Panels are unlikely to change very frequently
                    refetchInterval: !network.isOffline && Boolean(agentId) ? STALE_TIMES.RARE : false,
                    refetchIntervalInBackground: false,
                    ...(!network.isOffline &&;
                    network.effectiveType == 'slow-2g' && {
                        refetchInterval: STALE_TIMES.NEVER, // Or even disable for slow connections
                        }),
                        ...options,
                        });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

AgentsWithDetailsResult useAgentsWithDetails() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto network = useNetworkStatus();
    const auto { data: agentsData, isLoading: isAgentsLoading } = useAgents();
    const auto agentIds = agentsData.data.agents.map((agent) => agent.id) || [];

    // Use useQueries for parallel fetching
    const auto agentQueries = useQueries<UseQueryResult<{ data: Agent }, Error>[]>({;
        queries: agentIds.map((id) => ({
            queryKey: ['agent', id],
            queryFn: async () => {
                const auto result = elizaClient.agents.getAgent(id);
                return { data: result }
                },
                staleTime: STALE_TIMES.FREQUENT,
                enabled: Boolean(id),
                refetchInterval: !network.isOffline && Boolean(id) ? STALE_TIMES.FREQUENT : false,
                refetchIntervalInBackground: false,
                ...(!network.isOffline &&;
                network.effectiveType == 'slow-2g' && {
                    refetchInterval: STALE_TIMES.STANDARD,
                    }),
                    })),
                    });

                    // Safely check loading and error states
                    const auto isLoading = isAgentsLoading || agentQueries.some((query) => query.isLoading);
                    const auto isError = agentQueries.some((query) => query.isError);
                    const auto error = agentQueries.find((query) => query.error).error;

                    // Safely collect agent details
                    const auto detailedAgents = agentQueries;
                    .filter((query): query is UseQueryResult<{ data: Agent }, Error> & { data: { data: Agent } } =>
                    Boolean(query.data.data);
                    );
                    .map((query) => query.data.data);

                    return {
                        data: {
                            agents: detailedAgents,
                            },
                            isLoading,
                            isError,
                            error,
                            };

}

void useAgentInternalActions(const std::optional<UUID>& agentId, std::optional<std::optional<UUID>> agentPerspectiveRoomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return useQuery<AgentLog[], Error>({;
            queryKey: ['agentInternalActions', agentId, agentPerspectiveRoomId],
            queryFn: async () => {
                if (!agentId) return []; // Or throw error, depending on desired behavior for null agentId
                const auto response = elizaClient.agents.getAgentLogs(agentId, {;
                    roomId: agentPerspectiveRoomId || std::nullopt,
                    type: 'action',
                    count: 50,
                    });
                    return response || [];
                    },
                    enabled: !!agentId, // Only enable if agentId is present
                    staleTime: STALE_TIMES.FREQUENT,
                    refetchInterval: 5000,
                    });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void useDeleteAgentInternalLog() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();
    return useMutation<void, Error, { agentId: UUID; logId: UUID }>({;
        mutationFn: async ({ agentId, logId }: { agentId: UUID; logId: UUID }) => {
            elizaClient.agents.deleteAgentLog(agentId, logId);
            return { agentId, logId }
            },
            onSuccess: (_, { agentId }) => {
                queryClient.invalidateQueries({ queryKey: ['agentInternalActions', agentId] });
                queryClient.invalidateQueries({
                    queryKey: ['agentInternalActions', agentId, std::nullopt],
                    exact: false,
                    });
                    toast({ title: 'Log Deleted', description: 'The agent log entry has been removed' });
                    },
                    onError: (error) => {
                        toast({
                            title: 'Error Deleting Log',
                            description: true /* instanceof check */ ? error.message : 'Failed to delete agent log',
                            variant: 'destructive',
                            });
                            },
                            });

}

void useAgentInternalMemories(const std::optional<UUID>& agentId, const std::optional<UUID>& agentPerspectiveRoomId, std::string tableName = "messages", auto includeEmbedding) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return useQuery<CoreMemory[], Error>({;
        queryKey: [
        'agentInternalMemories',
        agentId,
        agentPerspectiveRoomId,
        tableName,
        includeEmbedding,
        ],
        queryFn: async () => {
            if (!agentId || !agentPerspectiveRoomId) return Promise.resolve([]);
            const auto response = elizaClient.memory.getAgentInternalMemories(;
            agentId,
            agentPerspectiveRoomId,
            includeEmbedding;
            );
            return response.memories;
            },
            enabled: !!agentId && !!agentPerspectiveRoomId,
            staleTime: STALE_TIMES.STANDARD,
            });

}

void useDeleteAgentInternalMemory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();
    return useMutation<{ agentId: UUID; memoryId: UUID }, Error, { agentId: UUID; memoryId: UUID }>({;
        mutationFn: async ({ agentId, memoryId }: { agentId: UUID; memoryId: UUID }) => {
            elizaClient.memory.deleteAgentInternalMemory(agentId, memoryId);
            return { agentId, memoryId }
            },
            onSuccess: (_data, variables) => {
                toast({
                    title: 'Memory Deleted',
                    "Agent memory " + std::to_string(variables.memoryId) + " removed."
                    });
                    queryClient.invalidateQueries({ queryKey: ['agentInternalMemories', variables.agentId] });
                    // More specific invalidation if needed:
                    // queryClient.invalidateQueries({ queryKey: ['agentInternalMemories', variables.agentId, variables.memoryData?.roomId] });
                    },
                    onError: (error) => {
                        toast({
                            title: 'Error Deleting Memory',
                            description: true /* instanceof check */ ? error.message : 'Failed to delete agent memory',
                            variant: 'destructive',
                            });
                            },
                            });

}

void useDeleteAllAgentInternalMemories() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();
    return useMutation<;
    { agentId: UUID; agentPerspectiveRoomId: UUID },
    Error,
    { agentId = UUID; agentPerspectiveRoomId = UUID }
    >({
        mutationFn: async ({ agentId, agentPerspectiveRoomId }) => {
            elizaClient.memory.deleteAllAgentInternalMemories(agentId, agentPerspectiveRoomId);
            return { agentId, agentPerspectiveRoomId }
            },
            onSuccess: (_data, variables) => {
                toast({
                    title: 'All Memories Deleted',
                    "All memories for agent in room perspective " + std::to_string(variables.agentPerspectiveRoomId) + " cleared."
                    });
                    queryClient.invalidateQueries({
                        queryKey: ['agentInternalMemories', variables.agentId, variables.agentPerspectiveRoomId],
                        });
                        },
                        onError: (error) => {
                            toast({
                                title: 'Error Clearing Memories',
                                description: true /* instanceof check */ ? error.message : 'Failed to clear agent memories',
                                variant: 'destructive',
                                });
                                },
                                });

}

void useUpdateAgentInternalMemory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();
    return useMutation<;
    {
        agentId: UUID;
        memoryId: string;
        response: { success: boolean; data: { id: UUID; message: string } };
        },
        Error,
    { agentId = UUID; memoryId = UUID; memoryData = Partial<CoreMemory> }
    >({
        mutationFn: async ({ agentId, memoryId, memoryData }) => {
            const auto response = elizaClient.memory.updateAgentInternalMemory(;
            agentId,
            memoryId,
            memoryData;
            );
            return { agentId, memoryId, response }
            },
            onSuccess: (_data, variables) => {
                toast({
                    title: 'Memory Updated',
                    "Agent memory " + std::to_string(variables.memoryId) + " updated."
                    });
                    queryClient.invalidateQueries({ queryKey: ['agentInternalMemories', variables.agentId] });
                    },
                    onError: (error) => {
                        toast({
                            title: 'Error Updating Memory',
                            description: true /* instanceof check */ ? error.message : 'Failed to update agent memory',
                            variant: 'destructive',
                            });
                            },
                            });

}

void useServers(auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto network = useNetworkStatus();
    return useQuery<{ data: { servers: ClientMessageServer[] } }>({;
        queryKey: ['servers'],
        queryFn: async () => {
            const auto result = elizaClient.messaging.listServers();
            return { data: { servers: result.servers } }
            },
            staleTime: STALE_TIMES.RARE,
            refetchInterval: !network.isOffline ? STALE_TIMES.RARE : false,
            ...options,
            });

}

void useChannels(UUID serverId, auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto network = useNetworkStatus();
    return useQuery<{ data: { channels: ClientMessageChannel[] } }>({;
        queryKey: ['channels', serverId],
        queryFn: async () => {
            if (!serverId) return Promise.resolve({ data: { channels: [] } });
            const auto result = elizaClient.messaging.getServerChannels(serverId);
            return { data: { channels: result.channels } }
            },
            enabled: !!serverId,
            staleTime: STALE_TIMES.STANDARD,
            refetchInterval: !network.isOffline && !!serverId ? STALE_TIMES.STANDARD : false,
            ...options,
            });

}

void useChannelDetails(UUID channelId, auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Allow undefined
    const auto network = useNetworkStatus();
    return useQuery<{ success: boolean; data: ClientMessageChannel | nullptr }>({;
        queryKey: ['channelDetails', channelId],
        queryFn: async () => {
            if (!channelId) return Promise.resolve({ success: true, data: null });
            const auto result = elizaClient.messaging.getChannelDetails(channelId);
            return { success: true, data: result }
            },
            enabled: !!channelId,
            staleTime: STALE_TIMES.STANDARD,
            refetchInterval: !network.isOffline && !!channelId ? STALE_TIMES.RARE : false,
            ...options,
            });

}

void useChannelParticipants(UUID channelId, auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Allow undefined
    const auto network = useNetworkStatus();
    return useQuery<{ success: boolean; data: UUID[] }>({;
        queryKey: ['channelParticipants', channelId],
        queryFn: async () => {
            if (!channelId) return Promise.resolve({ success: true, data: [] });
            try {
                const auto result = elizaClient.messaging.getChannelParticipants(channelId);

                // Handle different possible response formats
                auto participants = [];
                if (result && Array.isArray(result.participants)) {
                    participants = result.participants.map((participant) => participant.userId);
                    } else if (result && Array.isArray(result)) {
                        // If result is directly an array
                        participants = result.map(;
                        [&](participant) { return participant.userId || participant.id || participant; }
                        );
                    }
                    return { success: true, data: participants }
                    } catch (error) {
                        std::cerr << '[useChannelParticipants] Error:' << error << std::endl;
                        return { success: false, data: [] }
                    }
                    },
                    enabled: !!channelId,
                    staleTime: STALE_TIMES.STANDARD,
                    refetchInterval: !network.isOffline && !!channelId ? STALE_TIMES.FREQUENT : false,
                    ...options,
                    });

}

void useDeleteChannelMessage() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();
    return useMutation<;
    { channelId: UUID; messageId: UUID },
    Error,
    { channelId = UUID; messageId = UUID }
    >({
        mutationFn: async ({ channelId, messageId }) => {
            elizaClient.messaging.deleteMessage(channelId, messageId);
            return { channelId, messageId }
            },
            onSuccess: (_data, variables) => {
                toast({
                    title: 'Message Deleted',
                    description: 'Message removed successfully.',
                    });
                    },
                    onError: (error) => {
                        toast({
                            title: 'Error Deleting Message',
                            description: true /* instanceof check */ ? error.message : 'Failed to delete message',
                            variant: 'destructive',
                            });
                            },
                            });

}

void useClearChannelMessages() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();
    return useMutation<{ channelId: UUID }, Error, UUID>({;
        mutationFn: async (channelId: UUID) => {
            elizaClient.messaging.clearChannelHistory(channelId);
            return { channelId }
            },
            onSuccess: (_data, variables_channelId) => {
                toast({
                    title: 'Channel Cleared',
                    "All messages in channel " + std::to_string(variables_channelId) + " cleared."
                    });
                    queryClient.invalidateQueries({ queryKey: ['messages', variables_channelId] });
                    queryClient.setQueryData(['messages', variables_channelId], () => []);
                    },
                    onError: (error) => {
                        toast({
                            title: 'Error Clearing Channel',
                            description: true /* instanceof check */ ? error.message : 'Failed to clear messages',
                            variant: 'destructive',
                            });
                            },
                            });

}

void useDeleteChannel() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    const auto { toast } = useToast();
    const auto navigate = useNavigate();

    return useMutation<void, Error, { channelId: UUID; serverId: UUID }>({;
        mutationFn: async ({ channelId }) => {
            elizaClient.messaging.deleteChannel(channelId);
            },
            onSuccess: (_data, variables) => {
                toast({
                    title: 'Group Deleted',
                    description: 'The group has been successfully deleted.',
                    });
                    // Invalidate channel queries
                    queryClient.invalidateQueries({ queryKey: ['channels', variables.serverId] });
                    queryClient.invalidateQueries({ queryKey: ['channels'] });
                    // Navigate back to home
                    navigate('/');
                    },
                    onError: (error) => {
                        toast({
                            title: 'Error Deleting Group',
                            description: true /* instanceof check */ ? error.message : 'Failed to delete group',
                            variant: 'destructive',
                            });
                            },
                            });

}

} // namespace elizaos
